local Config = {}

Config.EngineDefaultGroup = "Engine"

Config.RootDir = assert(_MAIN_SCRIPT_DIR)

Config.BuildRoot = path.join(Config.RootDir, "Build")

Config.OutputDir = path.join(Config.BuildRoot, "%{cfg.platform}_%{cfg.buildcfg}_%{cfg.toolset}", "%{wks.name}")

Config.BinariesDir = path.join(Config.OutputDir, "Binaries")
Config.LibraryDir = path.join(Config.OutputDir, "Library")
Config.ObjectDir = path.join(Config.OutputDir, "Intermediate")

Config.ProjectLocation = "./"

Config.SourceDir = path.join(Config.RootDir, "Source")

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