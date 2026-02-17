local Config = require "Config"
local Engine = require "Engine"

require "Dependencies"

Oyl.Package = {}
local Package = Oyl.Package

---@class (exact) GitDesc
---@field Url string
---@field Revision? string
---@field Tag? string

---@class (exact) Package
---@field Name? string
---@field Git GitDesc
---@field Kind string
---@field Files? [string]
---@field ProjectDir? string
---@field IncludeDirs? [string]
---@field CustomProperties? fun()
---@field DependantProperties? fun(package: Package)

function Oyl.Package.GenerateProjects()
    for name, package in pairs(Oyl.Packages) do
        Package.SetupVarsInPackage(name, package)
        
        os.mkdir(package.ProjectDir)
        local cwd = os.getcwd()
        os.chdir(package.ProjectDir)

        project(package.Name)
            Engine.ApplyCommonCppSettings()
            kind(package.Kind)
            includedirs(package.IncludeDirs)
            warnings "Off"
            defines {
                -- Warning Silence Defines
                "_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING", -- Silence warning in spdlog
            }

            Engine.FilterStandalone()
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

---@param name string
---@param package Package
function Package.SetupVarsInPackage(name, package)
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
    else -- If IncludeDirs isn't manually defined, set it to the include folder of the package
        local includeDirs = os.matchdirs(package.ProjectDir .. "/**include/")
        if #includeDirs ~= 0 then
            package.IncludeDirs = { "%{wks.location}/" .. includeDirs[1] }
        else
            package.IncludeDirs = {}
        end
    end
end

newoption {
    trigger = "init-packages",
    category = "packages",
    description = "fetch all packages",
}

newoption {
    trigger = "reinit-packages",
    category = "packages",
    description = "Clear and re-fetch all packages",
}

newoption {
    trigger = "dryrun",
    category = "packages",
    description = "Dry run cleaning and fetching packages",
}

function Package.UpdatePackageCache()
    local reinit = _OPTIONS["reinit-packages"] and not _OPTIONS["premake-check"]
    if (reinit) then
        Package.CleanPackageCache()
    end
    
    local init = _OPTIONS["init-packages"] or (_ACTION ~= nil and string.sub(_ACTION, 1, 2) == "vs")
    if (init or reinit) then
        Package.FetchPackages()
    end
end

function Oyl.Package.FetchPackages()
    for _, package in pairs(Oyl.Packages) do
        local packageExists = os.isdir(package.ProjectDir)
        if not packageExists then
            if package.Git then
                Package.GitClone(package)
            end
        end
    end
end

---@param package? string
function Package.CleanPackageCache(package)
    local matchString = "*"
    if (package) then
        if (string.find(package, [[(\.\.)|\/|\\]], 1, true)) then
            print(string.format("Failed to clean package with invalid name \"\"", package));
            return
        end
        matchString = package
    end

    local packagesToRemove = os.matchdirs(Config.PackagesDir .. matchString)
    if #packagesToRemove ~= 0 then
        for _, dir in pairs(packagesToRemove) do
            if (_OPTIONS["dryrun"]) then
                printf("[DRYRUN]\tWould Clean %s...", dir)
            else
                printf("\tCleaning %s...", dir)
                os.execute(os.translateCommands("{RMDIR} " .. dir))
            end
        end
    else
        print("No Packages in Cache to remove.")
    end
end

---@param package Package
function Package.GitClone(package)
    local SUPPRESS_COMMAND_OUTPUT = (os.host() == "windows" and "> nul 2>&1" or "> /dev/null 2>&1")
    
    if (_OPTIONS["dryrun"]) then
        printf("[DRYRUN] Would Clone Git package \"%s\" from \"%s\"...", package.Name, package.Git.Url)
        if package.Git.Revision then
            local revision = package.Git.Revision
            printf("[DRYRUN]\tWould fetch revision \"%s\"...", revision)
        elseif package.Git.Tag then
            local tag = package.Git.Tag
            printf("[DRYRUN]\tWould fetch tag \"%s\"...", tag)
        end
        return
    end 

    printf("Cloning Git package \"%s\" from \"%s\"...", package.Name, package.Git.Url)

    -- Clone the repository and cd into it https://stackoverflow.com/a/63786181
    os.executef(
        "git clone -q --filter=blob:none -n --depth 1 %s %s %s", 
        package.Git.Url,
        package.ProjectDir,
        SUPPRESS_COMMAND_OUTPUT
    )
    local cwd = os.getcwd()
    os.chdir(package.ProjectDir)

    if package.Files then
        -- Clone specified files and license only
        local files = "!/* /LICENSE*"
        for k, file in pairs(package.Files) do
            if file:sub(1, 1) ~= "/" then
                file = "/" .. file
            end
            files = files .. " " .. file
        end

        os.executef("git sparse-checkout set --no-cone %s %s", files, SUPPRESS_COMMAND_OUTPUT)
    end

    -- If a specific revision or tag is specified, point to it
    if package.Git.Revision then
        local revision = package.Git.Revision
        printf("\tFetching revision \"%s\"...", revision)
        os.executef("git fetch -q --depth 1 origin %s %s", revision, SUPPRESS_COMMAND_OUTPUT)
        os.executef("git checkout -q --no-progress %s %s", revision, SUPPRESS_COMMAND_OUTPUT)
    elseif package.Git.Tag then
        local tag = package.Git.Tag
        printf("\tFetching tag \"%s\"...", tag)
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

return Package