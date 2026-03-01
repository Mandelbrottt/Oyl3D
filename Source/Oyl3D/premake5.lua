local Config = require "Config"
local Engine = require "Engine"
local Package = require "Package"
local Check = require "CheckProject"

workspace(Config.Name) do
    location "./"
    filename(Config.Name .. "_" .. _ACTION)

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

    startproject(Config.ShortName .. ".Entry")
    
    Engine.GenerateProjects()

    group "Dependencies"
        Package.GenerateProjects()

    Check.GenerateProject()
end