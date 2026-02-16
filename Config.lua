Config = {}

Config.Name = "Oyl3D"
Config.ShortName = "Oyl"

Config.TargetDir = "%{wks.location}/Build/Binaries/"
Config.LibraryDir = "%{wks.location}/Build/Library/"
Config.ObjectDir = "%{wks.location}/Build/Intermediate/"
Config.OutputDir = "%{cfg.buildcfg}_%{cfg.platform}_%{cfg.system}-%{cfg.architecture}_%{cfg.toolset}/"

Config.Utils = {}
Config.Utils.TargetDir = "%{wks.location}/Build/Binaries/Utils/"
Config.Utils.OutputDir = "%{cfg.buildcfg}_%{cfg.system}-%{cfg.architecture}/"

Config.ProjectLocation = "./"

Config.SourceDir = "Source/"
Config.PackagesDir = Config.SourceDir .. "Packages/"

Config.Configurations = {}
Config.Configurations.Debug = "Debug"
Config.Configurations.Development = "Development"
Config.Configurations.Profile = "Profile"
Config.Configurations.Distribution = "Distribution"

Config.Platforms = {}
Config.Platforms.Editor = "Editor"
Config.Platforms.Standalone = "Standalone"