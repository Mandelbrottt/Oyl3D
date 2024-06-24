Assemblies = {}

-- AssemblyDefinition: {
--      Group: string,
--      Language: string,
--      Kind: string,
--      Pch: string
--      Dependencies: table[string]
--      Properties: function
-- }
function EngineAssembly(assemblyDefinition)
    local projectDir = script_dir(3)
    local projectName = path.getname(projectDir)

    assemblyDefinition.Dir = projectDir
    assemblyDefinition.Name = projectName
    assemblyDefinition.ProjectName = string.format("%s.%s", Config.ShortName, projectName)
    assemblyDefinition.Group = assemblyDefinition.Group or ""
    assemblyDefinition.Kind = assemblyDefinition.Kind or premake.UTILITY

    if assemblyDefinition.Group then
        group(assemblyDefinition.Group)
    end

    project(assemblyDefinition.ProjectName)
        if assemblyDefinition.Language == premake.CPP then
            applyCommonCppSettings(assemblyDefinition)
        else
            premake.error(string.format("Invalid language \"%s\" in project \"%s\"", assemblyDefinition.Language, projectName))
        end

        kind(assemblyDefinition.Kind)
        if assemblyDefinition.Kind == premake.SHAREDLIB then
            filterStandalone()
                kind(premake.STATICLIB)
        end

        filter {}

        files { "**.lua" }

        removefiles { "**/Platform/**" }

        filter "system:windows"
            files { "**/Platform/Windows/**" }

        filter {}
        if assemblyDefinition.Properties then
            assemblyDefinition.Properties()
        end
        filter {}
    group ""

    Assemblies[assemblyDefinition.Name] = assemblyDefinition
end