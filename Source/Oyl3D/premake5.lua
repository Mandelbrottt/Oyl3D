local Check = require "CheckProject"
local Config = require "Config"
local Engine = require "Engine"

---@type Package.Projects
local Packages = dofile "Packages.lua"

workspace "Oyl3D"; do
	filename("%{wks.name}")

	configurations {
		Config.Configurations.Debug,
		Config.Configurations.Development,
		Config.Configurations.Profile,
		Config.Configurations.Distribution,
	}

	platforms {
		Config.Platforms.Editor,
		Config.Platforms.Standalone,
	}

	---@cast Packages Package.Project
	Engine.GenerateProjects {
		Packages = Packages
	}
	
	Check.GenerateProject {
		"--workspace=oyl3d"
	}
end
