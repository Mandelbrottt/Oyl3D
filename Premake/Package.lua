local Config = require "Config"
local Engine = require "Engine"

require "Dependencies"

Oyl.Package = {}
local Package = Oyl.Package

---@class (exact) ArchiveDesc
---@field Url string

---@class (exact) GitDesc
---@field Url string
---@field Revision? string
---@field Tag? string

---@class Package
---@field GenerateProject? boolean
---@field Name? string
---@field Archive? ArchiveDesc
---@field Git? GitDesc
---@field Kind? string
---@field Files? [string]
---@field ProjectDir? string Path to the project relative to workspace
---@field IncludeDirs? [string]
---@field CustomProperties? fun()
---@field DependantProperties? fun(package: Package)

function Oyl.Package.GenerateProjects()
    for _, package in pairs(Oyl.Packages) do
        os.mkdir(package.ProjectDir)
        local cwd = os.getcwd()
        os.chdir(package.ProjectDir)

        if package.GenerateProject ~= false then
            project(package.Name)
                Engine.ApplyCommonCppSettings()
                kind(package.Kind)
                includedirs(package.IncludeDirs)
                warnings "Off"

                Engine.FilterStandalone()
                if package.Kind == premake.SHAREDLIB then
                    kind(premake.STATICLIB)
                end

                filter {}
                if package['CustomProperties'] then
                    package.CustomProperties()
                end
                filter {}
            project "*"
        end
        os.chdir(cwd)
    end
end

---@param name string
---@param package Package
function Package.SetupVarsInPackage(name, package)
    -- Default to Utility if no kind provided
    package.Kind = package.Kind or "Utility"

    package.Name = name
    if (not package.ProjectDir) then
        package.ProjectDir = Config.PackagesDir .. name .. "/"
    end

    if package.IncludeDirs ~= nil then
        for i, includeDir in ipairs(package.IncludeDirs) do
            includeDir = "%{wks.location}/" .. package.ProjectDir .. includeDir
            package.IncludeDirs[i] = includeDir
        end
    else -- If IncludeDirs isn't manually defined, set it to the include folder of the package
        local includeDirsString = package.ProjectDir .. "/**include/"
        package.IncludeDirs = { "%{wks.location}/" .. includeDirsString }
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
    for name, package in pairs(Oyl.Packages) do
        Package.SetupVarsInPackage(name, package)
    end
    
    local reinit = _OPTIONS["reinit-packages"] and not _OPTIONS["premake-check"]
    if (reinit) then
        Package.CleanPackageCache()
    end
    
    local init = _OPTIONS["init-packages"] or (_ACTION ~= nil and string.sub(_ACTION, 1, 2) == "vs")
    if (init or reinit) then
        Package.FetchPackages(Oyl.Packages)
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

---@param packages Package[]
function Oyl.Package.FetchPackages(packages)
    for name, package in pairs(packages) do
        local numFiles = os.match(package.ProjectDir .. "/*")
        if #numFiles == 0 then
            -- For now, trust the user won't add multiple types of packages
            if package.Git then
                Package.GitClone(package)
            end
            if package.Archive then
                Package.FetchArchive(package)
            end
        end
    end
end

local executeOrPrint = function(command)
    if (not _OPTIONS["dryrun"]) then
        term.pushColor(term.infoColor)
        io.write(("\t%s "):format(command))
        term.popColor()
        printf("in %s", os.getcwd())
        os.execute(command)
    else
        term.pushColor(term.infoColor)
        io.write(("\t%s "):format(command))
        term.popColor()
        printf("in %s", os.getcwd())
    end
end

---@param package Package
function Package.GitClone(package)
    
    local SUPPRESS_COMMAND_OUTPUT = (os.host() == "windows" and "> nul 2>&1" or "> /dev/null 2>&1")
    local Git = package.Git
    assert(Git ~= nil)

    printf("Cloning Git package \"%s\" from \"%s\"...", package.Name, Git.Url)

    executeOrPrint(string.format("git init %s %s", package.ProjectDir, SUPPRESS_COMMAND_OUTPUT))

    local cwd = os.getcwd()
    os.chdir(package.ProjectDir)

    executeOrPrint(string.format("git remote add origin %s %s", Git.Url, SUPPRESS_COMMAND_OUTPUT))

    if package.Files then
        -- Clone specified files and license only
        local files = "!/* /LICENSE*"
        for k, file in pairs(package.Files) do
            if file:sub(1, 1) ~= "/" then
                file = "/" .. file
            end
            files = files .. " " .. file
        end

        executeOrPrint(string.format("git sparse-checkout set --no-cone %s %s", files, SUPPRESS_COMMAND_OUTPUT))
    end

    -- If a specific revision or tag is specified, point to it
    if Git.Revision then
        local revision = Git.Revision
        printf("\tFetching revision \"%s\"...", revision)
        executeOrPrint(string.format("git fetch -q --depth 1 origin %s %s", revision, SUPPRESS_COMMAND_OUTPUT))
        executeOrPrint(string.format("git checkout -q --no-progress %s %s", revision, SUPPRESS_COMMAND_OUTPUT))
    elseif Git.Tag then
        local tag = Git.Tag
        printf("\tFetching tag \"%s\"...", tag)
        executeOrPrint(string.format("git fetch -q --depth 1 origin refs/tags/%s:refs/tags/%s %s", tag, tag, SUPPRESS_COMMAND_OUTPUT))
        executeOrPrint(string.format("git checkout -q --no-progress %s %s", tag, SUPPRESS_COMMAND_OUTPUT))
    end

    -- Remove the .git folder as we're only using git to download the repository
    executeOrPrint(os.translateCommands("{RMDIR} .git"))
    os.chdir(cwd)

    term.pushColor(term.green)
    print(string.format("\tPackage \"%s\" cloned succesfully!", package.Name))
    term.popColor()
end

---@param package Package
function Package.FetchArchive(package)
    local Archive = package.Archive
    assert(Archive ~= nil)

    local baseUrl = string.explode(Archive.Url, "?")[1]
    local archiveFile = path.getname(baseUrl)

    os.mkdir(package.ProjectDir)
    local cwd = os.getcwd()
    os.chdir(package.ProjectDir)
    
    if Archive.Url and Archive.Url ~= "" then
        printf("Fetching Archive \"%s\" from %s", package.Name, Archive.Url)
        local result_str, response_code = http.download(Archive.Url, archiveFile)
        if result_str ~= "OK" then error(string.format("[%s] %s", response_code, result_str)) end
    else
        error(("Empty Archive URL Provided for package \"%s\"!"):format(package.Name))
    end

    printf("Extracting archive file \"%s\" into %s", archiveFile, package.ProjectDir)
    local tarCommand = ("tar -x -f '%s'"):format(archiveFile)
    if os.host() == premake.WINDOWS then
        executeOrPrint("pwsh -Command " .. tarCommand)
    else
        executeOrPrint(tarCommand)
    end
    os.remove(archiveFile)

    os.chdir(cwd)

    term.pushColor(term.green)
    print(string.format("\tPackage \"%s\" fetched succesfully!", package.Name))
    term.popColor()
end

return Package