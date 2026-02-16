SUPPRESS_COMMAND_OUTPUT = (os.host() == "windows" and "> nul 2>&1" or "> /dev/null 2>&1")

local reinitOptionTrigger = "reinit"

newoption {
    trigger = reinitOptionTrigger,
    description = "Reinitialize all packages"
}

local function cloneGitPackage(package)
    print(string.format("Cloning Git package \"%s\" from \"%s\"...", package.Name, package.Git.Url))

    -- Clone the repository and cd into it https://stackoverflow.com/a/63786181
    os.executef("git clone -q --filter=blob:none -n --depth 1 %s %s %s", package.Git.Url, package.ProjectDir, SUPPRESS_COMMAND_OUTPUT)
    local cwd = os.getcwd()
    os.chdir(package.ProjectDir)

    if package['Files'] then
        -- Clone specified files and license only
        local files = "!/* /LICENSE*"
        for k, file in pairs(package.Files) do
            if file[1] ~= "/" then
                file = "/" .. file
            end
            files = files .. " " .. file
        end

        os.executef("git sparse-checkout set --no-cone %s %s", files, SUPPRESS_COMMAND_OUTPUT)
    end

    -- If a specific revision or tag is specified, point to it
    if package.Git.Revision then
        local revision = package.Git.Revision
        print(string.format("\tFetching revision \"%s\"...", revision))
        os.executef("git fetch -q --depth 1 origin %s %s", revision, SUPPRESS_COMMAND_OUTPUT)
        os.executef("git checkout -q --no-progress %s %s", revision, SUPPRESS_COMMAND_OUTPUT)
    elseif package.Git.Tag then
        local tag = package.Git.Tag
        print(string.format("\tFetching tag \"%s\"...", tag))
        os.executef("git fetch -q --depth 1 origin refs/tags/%s:refs/tags/%s %s", tag, tag, SUPPRESS_COMMAND_OUTPUT)
        os.executef("git checkout -q --no-progress %s %s", tag, SUPPRESS_COMMAND_OUTPUT)
    end

    -- Remove the .git folder as we're only using git to download the repository
    os.execute(os.translateCommands("{RMDIR} .git"))
    os.chdir(cwd)

    term.pushColor(term.green)
    print(string.format("\tPackage \"%s\" cloned succesfully!", package.Name))
    term.popColor()
end

local function preProcessPackageTable(packages)
    for name, package in pairs(packages) do
        -- Default to Utility if no kind provided
        package.Kind = package.Kind or "Utility"

        local gitUrl = package.Git.Url
        name = name or path.getbasename(gitUrl)

        package['Name'] = name
        package['ProjectDir'] = Config.PackagesDir .. name .. "/"

        if package['IncludeDirs'] ~= nil then
            for i, includeDir in ipairs(package.IncludeDirs) do
                includeDir = "%{wks.location}/" .. package.ProjectDir .. includeDir
                package.IncludeDirs[i] = includeDir
            end
        end
    end
end

local function cleanPackagesCache()
    local depsToRemove = os.matchdirs(Config.PackagesDir .. "*")
    if #depsToRemove ~= 0 then
        print("Cleaning Package Cache...")
        for _, dir in pairs(depsToRemove) do
            print(string.format("\tCleaning %s...", dir))
            os.execute(os.translateCommands("{RMDIR} " .. dir))
        end
    else
        print("No Packages in Cache to remove.")
    end
end

local function cleanPackageInCache(packageToClean)
    if (string.find(packageToClean, [[(\.\.)|\/|\\]], 1, true)) then
        print(string.format("Failed to clean package with invalid name \"\"", packageToClean));
        return
    end
    local packagesToRemove = os.matchdirs(Config.PackagesDir .. packageToClean)
    if #packagesToRemove ~= 0 then
        print(string.format("Cleaning Package \"%s\" in Package Cache...", packageToClean))
        for _, dir in pairs(packagesToRemove) do
            os.execute(os.translateCommands("{RMDIR} " .. dir))
        end
    end
end

function processPackages(packages)
    preProcessPackageTable(packages)
    
    local reinitOption = _OPTIONS[reinitOptionTrigger]
    if reinitOption then
        if (reinitOption ~= "") then
            cleanPackageInCache(reinitOption)
        else
            cleanPackagesCache()
        end
    end

    for _, package in pairsByKeys(packages) do
        local packageExists = os.isdir(package.ProjectDir)
        if not packageExists then
            if package.Git then
                cloneGitPackage(package)
            end
        end

        -- If IncludeDirs isn't manually defined, set it to the include folder of the package
        if package['IncludeDirs'] == nil then
            local includeDirs = os.matchdirs(package.ProjectDir .. "/**include/")
            if #includeDirs ~= 0 then
                package.IncludeDirs = { "%{wks.location}/" .. includeDirs[1] }
            else
                package.IncludeDirs = {}
            end
        end
    end
end

function generatePackageProjects(packages)
    for _, package in pairsByKeys(packages) do
        local cwd = os.getcwd()
        os.chdir(package.ProjectDir)

        project(package.Name)
            applyCommonCppSettings()
            kind(package.Kind)
            includedirs(package.IncludeDirs)
            warnings "Off"
            defines {
                -- Warning Silence Defines
                "_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING", -- Silence warning in spdlog
            }
            
            filterStandalone()
            if package.Kind == premake.SHAREDLIB then
                kind(premake.STATICLIB)
            end
            
            filter {}
            if package['CustomProperties'] then
                package.CustomProperties()
            end
            filter {}

        os.chdir(cwd)
    end
    project "*"
end