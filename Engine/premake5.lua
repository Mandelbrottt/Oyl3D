Dirs = {}

Dirs.Target = "%{wks.location}/Build/Binaries/"
Dirs.Object = "%{wks.location}/Build/Intermediate/"

-- Dirs.Output = "Build/%{cfg.system}-%{cfg.architecture}-%{cfg.buildcfg}/"
Dirs.Output = "%{cfg.system}_%{cfg.architecture}_%{cfg.buildcfg}/"

-- Dirs.Project = "%{wks.location}/Intermediate/Refly/"
Dirs.Project = "./"

-- Dirs.ThirdPartyProject = "%{wks.location}/Intermediate/Refly/"
Dirs.ThirdPartyProject = "./"

Configs        = {}
Configs.Editor = {}

Configs.Postfix        = "-Standalone"
Configs.Editor.Postfix = "-Editor"

local DebugConfigName       = "Debug"
local DevelopmentConfigName = "Development"
local ReleaseConfigName     = "Release"

Configs.Debug        = DebugConfigName .. Configs.Postfix
Configs.Editor.Debug = DebugConfigName .. Configs.Editor.Postfix

Configs.Development        = DevelopmentConfigName .. Configs.Postfix
Configs.Editor.Development = DevelopmentConfigName .. Configs.Editor.Postfix

Configs.Release        = ReleaseConfigName .. Configs.Postfix
Configs.Editor.Release = ReleaseConfigName .. Configs.Editor.Postfix

function filterEditorOnly() 
	filter("configurations:not *" .. Configs.Postfix .. " or *" .. Configs.Editor.Postfix)
end

function ApplyCommonCppSettings() 
	language "C++"
    cppdialect "C++17"
	staticruntime "off"
    floatingpoint "fast"
		
	filter("configurations:" .. Configs.Debug .. "*")
		optimize "off"
		runtime "debug"
		defines { "REFLY_DEBUG=1" }
		
	filter("configurations:" .. Configs.Development .. "*")
		optimize "debug"
		runtime "release"
		defines { "REFLY_DEVELOPMENT=1" }
		
	filter("configurations:" .. Configs.Release .. "*")
		optimize "on"
		runtime "release"
		defines { "REFLY_RELEASE=1" }

	filterEditorOnly()
		defines { "REFLY_EDITOR=1" }

	filter {}
		
	flags {
		"FatalWarnings",
		"MultiProcessorCompile",
	}    
end

workspace "Refly-Engine"
	location "./"

	configurations {
		Configs.Editor.Debug,
		Configs.Editor.Development,
		Configs.Editor.Release,

		Configs.Debug,
		Configs.Development,
		Configs.Release,
	}

	platforms {
		-- "x86",
		"x64",
	}

	filter "platforms:*32 or *86"
		architecture "x86"
	filter "platforms:*64"
		architecture "x64"
	filter {}

	startproject "ReflyEntry"

	group "Refly"
		include "Source/Engine/"
		include "Source/Editor/"
		include "Source/Entry/"
	group ""

	group "ThirdParty"


	group ""

	