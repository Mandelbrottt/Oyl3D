local Config = {}

Config.EngineDefaultGroup = "Engine"

Config.RootDir = assert(_MAIN_SCRIPT_DIR)

Config.BuildDir = ("%s/Build"):format(Config.RootDir)

Config.TargetDir = ("%s/Binaries/"):format(Config.BuildDir)
Config.LibraryDir = ("%s/Library/"):format(Config.BuildDir)
Config.ObjectDir = ("%s/Intermediate/"):format(Config.BuildDir)
Config.OutputDir = "%{wks.name}/%{cfg.platform}_%{cfg.buildcfg}_%{cfg.toolset}/"

Config.Utils = {}
Config.Utils.TargetDir = ("%s/Utils/"):format(Config.TargetDir)
Config.Utils.OutputDir = "%{cfg.buildcfg}_%{cfg.system}-%{cfg.architecture}/"

Config.ProjectLocation = "./"

Config.SourceDir = "Source/"
Config.ToolsDir = "Tools/"

Config.PackageCacheDir = path.join(Config.RootDir, "Packages")

Config.Configurations = {}
Config.Configurations.Debug = "Debug"
Config.Configurations.Development = "Development"
Config.Configurations.Profile = "Profile"
Config.Configurations.Distribution = "Distribution"

Config.Platforms = {}
Config.Platforms.Editor = "Editor"
Config.Platforms.Standalone = "Standalone"

return Config