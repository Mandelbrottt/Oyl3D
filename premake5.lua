premake.path = ("%s;%s/Premake"):format(premake.path, _MAIN_SCRIPT_DIR)

local Config = require "Config"

local Check = require "CheckProject"
local Engine = require "Engine"
local Package = require "Package"

require "Actions/Clean"
require "Overrides"

Package.UpdatePackageCache()

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

    group "Dependencies"
        Package.GenerateProjects()

    group "Engine"
        Engine.GenerateProjects()

    group ""
        Check.Generate()
end