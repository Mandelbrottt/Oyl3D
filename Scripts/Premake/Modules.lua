Modules = {}

-- ModuleDefinition: {
--      Group: string,
--      Language: string,
--      Kind: string,
--      Pch: string
--      Dependencies: table[string]
--      Properties: function
-- }
function EngineModule(moduleDefinition)
    local projectDir = script_dir(3)
    local projectName = path.getname(projectDir)

    moduleDefinition.Dir = projectDir
    moduleDefinition.Name = projectName
    moduleDefinition.ProjectName = string.format("%s.%s", Config.ShortName, projectName)
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
        if moduleDefinition.Kind == premake.SHAREDLIB then
            filterStandalone()
                kind(premake.STATICLIB)
        end

        filter {}

        removefiles { "**/*windows*" }
        removefiles { "**/*unix*" }
        removefiles { "**/*linux*" }

        filter "system:windows"
            files { "**/*windows*" }

        filter {}
        if moduleDefinition.Properties then
            moduleDefinition.Properties()
        end
        filter {}
    group ""

    Modules[moduleDefinition.Name] = moduleDefinition
end