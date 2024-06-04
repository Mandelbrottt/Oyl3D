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