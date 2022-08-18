local SourceDir  = "Source/"
local BaseIncludeDir = "%{wks.location}/" .. SourceDir

Refly = {
    Name = "Refly",
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
    ThirdParty = {
        Name = "ThirdParty",
        IncludeDir = BaseIncludeDir .. "ThirdParty/",
        ProjectDir = SourceDir .. "ThirdParty/"
    },
    ZeroCheck = {
        Name = "ZERO_CHECK"
    }
}

Refly.Core.ProjectName = Refly.Name .. Refly.Core.Name
Refly.Entry.ProjectName = Refly.Name .. Refly.Entry.Name
Refly.Editor.ProjectName = Refly.Name .. Refly.Editor.Name

function validateDependencyCache(dependency)
    dependencyExists = os.isdir(dependency.ProjectDir)
    if not dependencyExists then
        -- Clone the repository and cd into it
        os.execute("git clone -n " .. dependency.Git.Url .. " " .. dependency.ProjectDir)
        local cwd = os.getcwd()
        os.chdir(dependency.ProjectDir)

        -- If a specific revision is specified, point to it
        if dependency.Git.Revision then
            os.execute("git checkout -q " .. dependency.Git.Revision)
        end

        -- Remove the .git folder as we're only using git to download the repository 
        os.execute(
            os.translateCommands("{RMDIR} .git")
        )
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
        depsToRemove = os.matchdirs(Refly.ThirdParty.ProjectDir .. "*")
        if #depsToRemove ~= 0 then
            print("Removing Dependency Cache...")
            for _, dir in pairs(depsToRemove) do
                os.rmdir(dir)
            end
        end
    end

    for _, dependency in pairs(Dependencies) do
        -- Default to StaticLib if none provided
        dependency.Linking = dependency.Linking or "StaticLib"
        
        local gitUrl = dependency.Git.Url
        local name = path.getbasename(gitUrl)
        
        dependency['Name'] = name
        dependency['ProjectDir'] = Refly.ThirdParty.ProjectDir .. name .. "/"

        validateDependencyCache(dependency)
        
        local cwd = os.getcwd()
        os.chdir(dependency.ProjectDir)

        local includeDirs = os.matchdirs("**include/")
        dependency['IncludeDir'] = "%{wks.location}/" .. dependency.ProjectDir .. includeDirs[1]
        
        project(dependency.Name)
            applyCommonCppSettings()
            removelinks { dependency.Name }
            kind(dependency.Linking)
            dependency.CustomProperties()

        os.chdir(cwd)
    end
end

function filterEditorOnly(filterCallback)
    filter("configurations:not *" .. Config.Postfix .. " or *" .. Config.Editor.Postfix)
        filterCallback()
    filter {}
end

function defineInsideMacro(projectName)
    return string.upper(
        string.format("_INSIDE_%s_%s=1", Refly.Name, projectName)
    )
end

function applyCommonCppSettings(projectConfig)
    language "C++"
    cppdialect "C++17"
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
    }

    links {
        Refly.ZeroCheck.Name
    }

    for _, dependency in pairs(Dependencies) do
        links { dependency.Name }
        includedirs { dependency.IncludeDir }
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

    filter("configurations:" .. Config.Debug.Prefix .. "*")
        optimize "off"
        runtime "debug"
        defines { string.upper(Refly.Name) .. "_DEBUG=1" }

    filter("configurations:" .. Config.Development.Prefix .. "*")
        optimize "debug"
        runtime "release"
        defines { string.upper(Refly.Name) .. "_DEVELOPMENT=1" }

    filter("configurations:" .. Config.Release.Prefix .. "*")
        optimize "on"
        runtime "release"
        defines { string.upper(Refly.Name) .. "_RELEASE=1" }
        
    filterEditorOnly(function()
        defines { string.upper(Refly.Name) .. "_EDITOR=1" }
    end)
end

local function mysplit (inputstr, sep)
    if sep == nil then
       sep = "%s"
    end
    local t={}
    for str in string.gmatch(inputstr, "([^"..sep.."]+)") do
       table.insert(t, str)
    end
    return t
end

-- https://stackoverflow.com/a/23535333
function scriptDir()
    local str = debug.getinfo(2, "S").source:sub(2)
    local path = str:match("(.*[/\\])") or "."
    local split = mysplit(path, "/\\")
    local last_dir
    for _, v in ipairs(split) do
        last_dir = v
    end
    return last_dir
end