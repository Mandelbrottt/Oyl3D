local suppressCommandOutput = (os.host() == "windows" and "> nul 2>&1" or "> /dev/null 2>&1")

local function cloneGitDependency(dependency)
    print(string.format("Cloning Git dependency \"%s\" from \"%s\"...", dependency.Name, dependency.Git.Url))

    -- Clone the repository and cd into it https://stackoverflow.com/a/63786181
    os.executef("git clone -q --filter=blob:none -n --depth 1 %s %s %s", dependency.Git.Url, dependency.ProjectDir, suppressCommandOutput)
    local cwd = os.getcwd()
    os.chdir(dependency.ProjectDir)

    if dependency['Files'] then
        -- Clone specified files and license only
        local files = "!/* /LICENSE*"
        for k, file in pairs(dependency.Files) do
            files = files .. " " .. file
        end

        os.executef("git sparse-checkout set --no-cone %s %s", files, suppressCommandOutput)
    end

    -- If a specific revision is specified, point to it
    local revision = ""
    if dependency.Git.Revision then
        revision = dependency.Git.Revision
        print(string.format("\tFetching revision \"%s\"...", revision))
        os.executef("git fetch -q --all --tags --depth 1 %s", suppressCommandOutput)
        os.executef("git checkout -q --no-progress %s %s", revision, suppressCommandOutput)
    end

    -- Remove the .git folder as we're only using git to download the repository
    os.execute(os.translateCommands("{RMDIR} .git"))
    os.chdir(cwd)

    term.pushColor(term.green)
    print(string.format("\tDependency \"%s\" cloned succesfully!", dependency.Name))
    term.popColor()
end

function validateDependencyCache(dependency)
    dependencyExists = os.isdir(dependency.ProjectDir)
    if not dependencyExists then
        if dependency.Git then
            cloneGitDependency(dependency)
        end
    end
end

local reinitOptionTrigger = "reinit"

newoption {
    trigger = reinitOptionTrigger,
    description = "Reinitialize all dependencies"
}

function generateDependencies()
    if _OPTIONS[reinitOptionTrigger] then
        depsToRemove = os.matchdirs(Rearm.Dependencies.ProjectDir .. "*")
        if #depsToRemove ~= 0 then
            print("Removing Dependency Cache...")
            for _, dir in pairs(depsToRemove) do
                os.execute(os.translateCommands("{RMDIR} " .. dir))
            end
        end
    end

    -- applyCommonCppSettings relies on the global Dependencies
    -- Make it an empty table while we generate the dependency projects so that they don't depend on each other
    local dependencies = Dependencies
    Dependencies = {}

    for _, dependency in pairs(dependencies) do
        -- Default to StaticLib if none provided
        dependency.Kind = dependency.Kind or "None"

        local gitUrl = dependency.Git.Url
        local name = dependency.Name or path.getbasename(gitUrl)

        dependency['Name'] = name
        dependency['ProjectDir'] = Rearm.Dependencies.ProjectDir .. name .. "/"

        validateDependencyCache(dependency)

        local cwd = os.getcwd()
        os.chdir(dependency.ProjectDir)

        if dependency['IncludeDirs'] == nil then
            local includeDirs = os.matchdirs("**include/")
            if #includeDirs ~= 0 then
                dependency.IncludeDirs = { "%{wks.location}/" .. dependency.ProjectDir .. includeDirs[1] }
            else
                dependency.IncludeDirs = {}
            end
        else
            for i, includeDir in ipairs(dependency.IncludeDirs) do
                includeDir = "%{wks.location}/" .. dependency.ProjectDir .. includeDir
                dependency.IncludeDirs[i] = includeDir
            end
        end

        project(dependency.Name)
        applyCommonCppSettings()
        kind(dependency.Kind)
        includedirs(dependency.IncludeDirs)
        warnings "Off"
        if dependency['CustomProperties'] then
            dependency.CustomProperties()
        end

        os.chdir(cwd)
    end

    Dependencies = dependencies
end
