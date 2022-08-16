require('vstudio')

local p = premake
local m = premake.vstudio.vc2010

premake.override(premake.vstudio.vc2010.elements, "projectReferences", function(base, prj, ref)
    if prj.clr ~= p.OFF or (m.isClrMixed(prj) and ref and ref.kind ~=p.STATICLIB) then
        return {
            m.referenceProject,
            m.referencePrivate,
            m.referenceOutputAssembly,
            m.referenceCopyLocalSatelliteAssemblies,
            m.referenceLinkLibraryDependencies,
            m.referenceUseLibraryDependences,
        }
    else
        return {
            m.referenceProject,
            function(prj, ref)
                m.element("Private", nil, "false")
            end,
            function(prj, ref)
                m.element("ReferenceOutputAssembly", nil, "false")
            end,
        }
    end
end)