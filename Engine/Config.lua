Config = {    
    TargetDir = "%{wks.location}/Build/Binaries/",
    ObjectDir = "%{wks.location}/Build/Intermediate/",
    
    -- Output = "Build/%{cfg.system}-%{cfg.architecture}-%{cfg.buildcfg}/"
    OutputDir = "%{cfg.system}_%{cfg.architecture}_%{cfg.buildcfg}/",
    
    -- Project = "%{wks.location}/Intermediate/Refly/"
    ProjectLocation = "./",
    
    -- ThirdPartyProject = "%{wks.location}/Intermediate/Refly/"
    ThirdPartyProjectLocation = "./",

    Postfix = "Standalone",
    Editor = {
        Postfix = "Editor",
    },
    Debug = {
        Prefix = "Debug"
    }, 
    Development = {
        Prefix = "Development"
    }, 
    Release = {
        Prefix = "Release"
    }
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
