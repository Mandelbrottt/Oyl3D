Config = {}

Config.Name = "Oyl3D"
Config.ShortName = "Oyl"

Config.TargetDir = "%{wks.location}/Build/Binaries/"
Config.LibraryDir = "%{wks.location}/Build/Library/"
Config.ObjectDir = "%{wks.location}/Build/Intermediate/"
Config.OutputDir = "%{cfg.buildcfg}_%{cfg.platform}_%{cfg.system}-%{cfg.architecture}_%{cfg.toolset}/"

Config.ProjectLocation = "./"

Config.SourceDir = "Source/"
Config.DependenciesDir = Config.SourceDir .. "Dependencies/"

Config.Configurations = {}
Config.Configurations.Debug = "Debug"
Config.Configurations.Development = "Development"
Config.Configurations.Profile = "Profile"
Config.Configurations.Distribution = "Distribution"

Config.Platforms = {}
Config.Platforms.Editor = "Editor"
Config.Platforms.Standalone = "Standalone"