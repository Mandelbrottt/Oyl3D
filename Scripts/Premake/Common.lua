local SourceDir  = "Source/"
local BaseIncludeDir = "%{wks.location}/" .. SourceDir

Rearm = {
    Name = "Rearm",
    IncludeDir = BaseIncludeDir,
    SourceDir = SourceDir,

    Core = {
        Name = "Core",
        IncludeDir = BaseIncludeDir .. "Core/",
        ProjectDir = SourceDir .. "Core/"
    },
    Entry = {
        Name = "Entry",
        IncludeDir = BaseIncludeDir .. "Entry/",
        ProjectDir = SourceDir .. "Entry/"
    },
    Editor = {
        Name = "Editor",
        IncludeDir = BaseIncludeDir .. "Editor/",
        ProjectDir = SourceDir .. "Editor/"
    },
    Dependencies = {
        Name = "Dependencies",
        IncludeDir = BaseIncludeDir .. "Dependencies/",
        ProjectDir = SourceDir .. "Dependencies/"
    },
    Exports = {
        Name = "Exports",
        IncludeDir = BaseIncludeDir .. "Exports/",
        ProjectDir = SourceDir .. "Exports/"
    },
    ZeroCheck = {
        Name = "ZERO_CHECK"
    }
}

Rearm.Core.ProjectName = Rearm.Name .. Rearm.Core.Name
Rearm.Entry.ProjectName = Rearm.Name .. Rearm.Entry.Name
Rearm.Editor.ProjectName = Rearm.Name .. Rearm.Editor.Name
Rearm.Exports.ProjectName = Rearm.Name .. Rearm.Exports.Name

function validateDependencyCache(dependency)
    dependencyExists = os.isdir(dependency.ProjectDir)
    if not dependencyExists then
        -- Clone the repository and cd into it https://stackoverflow.com/a/63786181
        os.execute("git clone --filter=blob:none -n --depth 1 --sparse " .. dependency.Git.Url .. " " .. dependency.ProjectDir)
        local cwd = os.getcwd()
        os.chdir(dependency.ProjectDir)

        if dependency['Files'] then
            local files = "!/* /LICENSE*"
            for k, file in pairs(dependency.Files) do
                files = files .. " " .. file
            end

            os.execute("git sparse-checkout set --no-cone " .. files)
        end

        -- If a specific revision is specified, point to it
        local revision = ""
        if dependency.Git.Revision then
            revision = dependency.Git.Revision
            os.execute("git fetch -q --all --tags --depth 1")
            os.execute("git checkout -q " .. revision)
        end

        -- Remove the .git folder as we're only using git to download the repository 
        os.execute(os.translateCommands("{RMDIR} .git"))
        os.chdir(cwd)
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

function filterEditorOnly(filterCallback)
    filter("configurations:not *" .. Config.Postfix .. " or *" .. Config.Editor.Postfix)
        filterCallback()
    filter {}
end

function defineInsideMacro(projectName)
    return string.upper(
        string.format("_INSIDE_%s_%s=1", Rearm.Name, projectName)
    )
end

function applyCommonCppSettings(projectConfig)
    filename("%{prj.name}_" .. _ACTION)
    language "C++"
    cppdialect "C++20"
    staticruntime "off"
    floatingpoint "fast"
    
    location (Config.ProjectLocation)
    targetdir(Config.TargetDir .. Config.OutputDir)
    objdir   (Config.ObjectDir .. Config.OutputDir)

    files {
        "./**.cpp",
        "./**.h",
        "./**.hpp",
        "./**.inl",
        "./**.ixx",
    }

    links {
        Rearm.ZeroCheck.Name
    }

    for _, dependency in pairs(Dependencies) do
        links { dependency.Name }
        includedirs(dependency.IncludeDirs)
    end

    if projectConfig then
        includedirs {
            projectConfig.IncludeDir,
            projectConfig.IncludeDir .. projectConfig.Name,
        }

        defines {
            defineInsideMacro(projectConfig.Name),
        }
    end

    flags {
        "FatalWarnings",
        "MultiProcessorCompile",
    }

    filter "toolset:msc*"
        disablewarnings {
            "5050",
            "4251", -- member needs dll-interface to be used by clients of class
            "4275", -- non dll-interface used as base for dll-interface
        }

    filter("configurations:" .. Config.Debug.Prefix .. "*")
        optimize "off"
        runtime "debug"
        defines { string.upper(Rearm.Name) .. "_DEBUG=1" }

    filter("configurations:" .. Config.Development.Prefix .. "*")
        optimize "debug"
        runtime "release"
        defines { string.upper(Rearm.Name) .. "_DEVELOPMENT=1" }

    filter("configurations:" .. Config.Release.Prefix .. "*")
        optimize "on"
        runtime "release"
        defines { string.upper(Rearm.Name) .. "_RELEASE=1" }
        
    filterEditorOnly(function()
        defines { string.upper(Rearm.Name) .. "_EDITOR=1" }
    end)

    filter {}
end