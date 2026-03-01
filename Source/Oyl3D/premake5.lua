local Config = require "Config"
local Engine = require "Engine"
local Package = require "Package"
local Check = require "CheckProject"

workspace "Oyl3D"; do
	location "./"
	filename("Oyl3D_" .. _ACTION)

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

	Engine.GenerateProjects()
	
	group "Dependencies"; do
		Package.GenerateProjects()
	end
	
	Check.GenerateProject()
	
	project "*"
	startproject(Oyl.Projects.Entry.Name)
end