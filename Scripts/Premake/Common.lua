local SourceDir  = "Source/"
local BaseIncludeDir = "%{wks.location}/" .. SourceDir

Rearm = {
    Name = "Rearm",
    IncludeDir = BaseIncludeDir,
    SourceDir = SourceDir,

    Core = {
        Name = "Core",
        IncludeDir = BaseIncludeDir .. "Core/",
        ProjectDir = SourceDir .. "Core/"
    },
    Rendering = {
        Name = "Rendering",
        IncludeDir = BaseIncludeDir .. "Rendering/",
        ProjectDir = SourceDir .. "Rendering/"
    },
    Editor = {
        Name = "Editor",
        IncludeDir = BaseIncludeDir .. "Editor/",
        ProjectDir = SourceDir .. "Editor/"
    },
    Entry = {
        Name = "Entry",
        IncludeDir = BaseIncludeDir .. "Entry/",
        ProjectDir = SourceDir .. "Entry/"
    },
    Dependencies = {
        Name = "Dependencies",
        IncludeDir = BaseIncludeDir .. "Dependencies/",
        ProjectDir = SourceDir .. "Dependencies/"
    },
    Premake = {
        Name = "Premake"
    }
}

Rearm.Core.ProjectName = Rearm.Name .. Rearm.Core.Name
Rearm.Rendering.ProjectName = Rearm.Name .. Rearm.Rendering.Name
Rearm.Editor.ProjectName = Rearm.Name .. Rearm.Editor.Name
Rearm.Entry.ProjectName = Rearm.Name .. Rearm.Entry.Name

include "Project.lua"
include "Dependencies.lua"