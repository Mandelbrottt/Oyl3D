Config = {}

Config.Name = "Rearm"

Config.TargetDir = "%{wks.location}/Build/Binaries/"
Config.ObjectDir = "%{wks.location}/Build/Intermediate/"
Config.OutputDir = "%{cfg.system}_%{cfg.architecture}_%{cfg.buildcfg}/"

Config.ProjectLocation = "./"

Config.SourceDir = "Source/"
Config.DependenciesDir = Config.SourceDir .. "Dependencies/"

Config.Postfix = "Standalone"
Config.Editor = {
    Postfix = "Editor"
}
Config.Debug = {
    Prefix = "Debug"
}
Config.Development = {
    Prefix = "Development"
}
Config.Release = {
    Prefix = "Release"
}


Config.Debug.ConfigName = Config.Debug.Prefix .. "-" .. Config.Postfix
Config.Debug.Editor = {}
Config.Debug.Editor.ConfigName = Config.Debug.Prefix .. "-" .. Config.Editor.Postfix

Config.Development.ConfigName = Config.Development.Prefix .. "-" .. Config.Postfix
Config.Development.Editor = {}
Config.Development.Editor.ConfigName = Config.Development.Prefix .. "-" .. Config.Editor.Postfix

Config.Release.ConfigName = Config.Release.Prefix .. "-" .. Config.Postfix
Config.Release.Editor = {}
Config.Release.Editor.ConfigName = Config.Release.Prefix .. "-" .. Config.Editor.Postfix
