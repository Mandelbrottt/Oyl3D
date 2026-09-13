local Config = require "Config"
local Package = require "Package"

local Oyl3D = require "Oyl3D"

local Packages = require "Oyl3D.Packages"

Oyl3D.Workspace "Oyl3D"; do
	filename "%{wks.name}"
	checkproject "On"

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

	group "Packages"; do
		Package.GenerateProjects {
			Packages = Packages,
			Defaults = {
				Cpp = Oyl3D.DefaultCppSettings
			}
		}
	end

	group ""; do
		Oyl3D.GenerateProjectsFromScripts()
	end
end
