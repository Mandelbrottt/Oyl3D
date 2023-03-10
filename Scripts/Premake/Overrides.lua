require('vstudio')

local p = premake
local m = premake.vstudio.vc2010

-- Override the project references section of the visual studio project
-- By default, visual studio will try to copy the build output of projects to their dependencies
-- This causes a failure if a project doesn't output any artifacts, such as a header only library
-- This change lets us reference assemblies in-place
premake.override(premake.vstudio.vc2010.elements, "projectReferences", function(base, prj, ref)
    -- Use default behaviour for SharedLib CLR projects
    if prj.clr ~= p.OFF or (m.isClrMixed(prj) and ref and ref.kind ~= p.STATICLIB) then
        return {
            m.referenceProject,
            m.referencePrivate,
            m.referenceOutputAssembly,
            m.referenceCopyLocalSatelliteAssemblies,
            m.referenceLinkLibraryDependencies,
            m.referenceUseLibraryDependences,
        }
    else
        -- See https://learn.microsoft.com/en-us/visualstudio/msbuild/common-msbuild-project-items?view=vs-2022
        -- for more documentation
        return {
            m.referenceProject,
            function(prj, ref)
                -- Don't copy the assembly to the dependant project
                m.element("Private", nil, "false")
            end,
            function(prj, ref)
                -- Don't include the assembly as a reference to dependant projects, since
                -- all assemblies are output to the same directory in the end
                m.element("ReferenceOutputAssembly", nil, "false")
            end,
        }
    end
end)

local projectModuleIfcs = {}

-- chdir into source directory to make string processing easier in next step
local oldcwd = os.getcwd()
os.chdir(_MAIN_SCRIPT_DIR .. "/" .. Rearm.SourceDir)
local allModuleInterfaces = os.matchfiles("**.ixx")
os.chdir(oldcwd)

for k, moduleInterfacePath in pairs(allModuleInterfaces) do
    local index, _ = string.find(moduleInterfacePath, "/")
    local project = string.sub(moduleInterfacePath, 1, index - 1)

    -- Convert from directory name to project name eg. Core -> RearmCore
    project = Rearm[project].ProjectName

    if not projectModuleIfcs[project] then
        projectModuleIfcs[project] = {}
    end

    local filename = path.getname(moduleInterfacePath)
    table.insert(projectModuleIfcs[project], filename .. ".ifc")
end

-- Visual Studio 2019 doesn't automatically reference module interfaces across DLL and project boundaries when linking
-- We need to manually find and add each module to the "AdditionalModuleDependencies" project setting for the compiler
-- to recognize and find the modules
premake.override(premake.vstudio.vc2010.elements, "clCompile", function(base, cfg)
    local clCompile = base(cfg)
    local function additionalModules(cfg)
        -- Don't add module dependencies to third party dependencies
        local dependenciesIndex = string.find(cfg.location, Rearm.Dependencies.ProjectDir)
        if dependenciesIndex ~= nil then
            return
        end

        local additionalModuleDependenciesString = ""
        for project, moduleTable in pairs(projectModuleIfcs) do
            -- Adding a project's own modules as dependencies would create a cyclical dependency
            if cfg.buildtarget.basename == project then
                goto continue
            end
            for _, moduleName in pairs(moduleTable) do
                additionalModuleDependenciesString = 
                    additionalModuleDependenciesString .. "$(IntDir)..\\" .. project .. "\\" .. moduleName .. ";"
            end
            ::continue::
        end
        
        if additionalModuleDependenciesString == "" then
            return
        end
        m.element("AdditionalModuleDependencies", nil, additionalModuleDependenciesString)
    end

    table.insert(clCompile, additionalModules)
    return clCompile
end)