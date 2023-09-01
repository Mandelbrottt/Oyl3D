-- Rearm = {
--     Name = "Rearm",
--     IncludeDir = BaseIncludeDir,
--     SourceDir = SourceDir,

--     Core = {
--         Name = "Core",
--         IncludeDir = BaseIncludeDir .. "Core/",
--         ProjectDir = SourceDir .. "Core/"
--     },
--     Rendering = {
--         Name = "Rendering",
--         IncludeDir = BaseIncludeDir .. "Rendering/",
--         ProjectDir = SourceDir .. "Rendering/"
--     },
--     Editor = {
--         Name = "Editor",
--         IncludeDir = BaseIncludeDir .. "Editor/",
--         ProjectDir = SourceDir .. "Editor/"
--     },
--     Entry = {
--         Name = "Entry",
--         IncludeDir = BaseIncludeDir .. "Entry/",
--         ProjectDir = SourceDir .. "Entry/"
--     },
--     Dependencies = {
--         Name = "Dependencies",
--         IncludeDir = BaseIncludeDir .. "Dependencies/",
--         ProjectDir = SourceDir .. "Dependencies/"
--     },
--     Premake = {
--         Name = "Premake"
--     }
-- }

-- Rearm.Core.ProjectName = Rearm.Name .. Rearm.Core.Name
-- Rearm.Rendering.ProjectName = Rearm.Name .. Rearm.Rendering.Name
-- Rearm.Editor.ProjectName = Rearm.Name .. Rearm.Editor.Name
-- Rearm.Entry.ProjectName = Rearm.Name .. Rearm.Entry.Name

Modules = {}

-- ModuleDefinition: {
--      Group: string,
--      Language: string,
--      Kind: string,
--      Pch: string
--      Dependencies: table[string]
--      Properties: function
-- }
function RearmModule(moduleDefinition)
    local projectDir = script_dir(3)
    local projectName = path.getname(projectDir)

    moduleDefinition.Dir = projectDir
    moduleDefinition.Name = projectName
    moduleDefinition.ProjectName = string.format("%s.%s", Config.Name, projectName)
    moduleDefinition.Group = moduleDefinition.Group or ""
    moduleDefinition.Kind = moduleDefinition.Kind or premake.UTILITY

    if moduleDefinition.Group then
        group(moduleDefinition.Group)
    end

    project(moduleDefinition.ProjectName)
        if moduleDefinition.Language == premake.CPP then
            applyCommonCppSettings(moduleDefinition)
        else
            premake.error(string.format("Invalid language \"%s\" in project \"%s\"", moduleDefinition.Language, projectName))
        end

        kind(moduleDefinition.Kind)

        if moduleDefinition.Properties then
            moduleDefinition.Properties()
        end
    filter {}
    group ""

    Modules[moduleDefinition.Name] = moduleDefinition
end