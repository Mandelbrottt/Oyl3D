TargetDir = "%{wks.location}/Build/Binaries/"
ObjectDir = "%{wks.location}/Build/Intermediate/"

OutputDir = "Build/%{cfg.system}/%{cfg.architecture}/%{cfg.buildcfg}"

-- EngineProjectDir = "%{wks.location}/Intermediate/Refly/"
EngineProjectDir = "./"

ThirdPartyProjectDir = "%{wks.location}/Intermediate/Refly/"

workspace "Refly-Engine"
	architecture "x64"

	location "./"

	configurations {
		"Debug-Editor",
		"Development-Editor",
		"Release-Editor",

		-- "Debug-Standalone",
		-- "Development-Standalone",
		-- "Release-Standalone",
	}

	startproject "Refly-Entry"

	group "Refly"
		include "Source/Entry/"
		include "Source/Engine/"
		include "Source/Editor/"
	group ""

	group "ThirdParty"


	group ""
	-- group "Refly"

	-- 	include "../Obelisk/Entry/"

	-- 	include "../Obelisk/Engine/"

	-- group ""