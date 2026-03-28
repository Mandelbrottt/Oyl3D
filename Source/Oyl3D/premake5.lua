local Check = require "CheckProject"
local Config = require "Config"
local Engine = require "Engine"

local Packages = require "Oyl3D.Packages"

workspace "Oyl3D"; do
	filename("%{wks.name}")
	
	configurations(table.keys(Config.Configurations))
	platforms(table.keys(Config.Platforms))

	Engine.GenerateProjects {
		Packages = Packages
	}
	
	Check.GenerateProject {
		"--workspace=oyl3d"
	}
end
