premake.path = ("%s;%s/Premake"):format(premake.path, _MAIN_SCRIPT_DIR)

local Config = require "Config"

require "Dependencies"

local Check = require "CheckProject"
local Engine = require "Engine"
local Package = require "Package"
local Library = require "Library"

require "Actions/Clean"
require "Overrides"

Package.SetupPackages()
Package.UpdatePackageCache()

Library.SetupLibraries()

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

    Check.Generate()
end