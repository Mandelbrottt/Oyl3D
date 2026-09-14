require "oyl3d"

local Config = require "Config"

local Oyl3D = require "Oyl3D_old"

local Packages = require "Oyl3D.Packages"

workspace "Oyl3D"; do
	filename "%{wks.name}"

	configurations {
		Config.Configurations.Debug,
		Config.Configurations.Development,
		Config.Configurations.Profile,
		Config.Configurations.Distribution,
	}

	platforms {
		Config.Platforms.Editor,
		Config.Platforms.Standalone
	}

	sourcedir "Source"

	projectdefaults {
		["C"] = Oyl3D.DefaultCppSettings,
		["C++"] = Oyl3D.DefaultCppSettings,
	}

	packageprojects {
		group = "Packages",
		packages = Packages,
	}

	checkproject "On"
end
