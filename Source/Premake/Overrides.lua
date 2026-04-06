require('vstudio')

local p = premake

-- Override the project references section of the visual studio project
-- By default, visual studio will try to copy the build output of projects to their dependencies
-- This causes a failure if a project doesn't output any artifacts, such as a header only library
-- This change lets us reference assemblies in-place
premake.override(premake.vstudio.vc2010.elements, "projectReferences", function(base, prj, ref)
	local m = premake.vstudio.vc2010

	local result = base(prj, ref)

	if not table.contains(result, m.referencePrivate) then
		table.insert(result,
			function(prj, ref)
				-- Don't copy the assembly to the dependant project
				m.element("Private", nil, "false")
			end)
	end

	if not table.contains(result, m.referenceProject) then
		table.insert(result,
			function(prj, ref)
				-- Don't include the assembly as a reference to dependant projects, since
				-- all assemblies are output to the same directory in the end
				m.element("ReferenceOutputAssembly", nil, "false")
			end)
	end

	return result
end)

premake.api.register {
	name = "buildtargetcustomizations",
	scope = "project",
	kind = "list:string",
	tokens = true,
	pathVars = true
}

-- Override premake buildcustomizations targets
premake.override(premake.vstudio.vc2010.elements, "importExtensionTargets", function(base, prj)
	local m = premake.vstudio.vc2010

	local result = base(prj)
	table.insert(result,
		function(prj)
			for i, build in ipairs(prj.buildtargetcustomizations) do
				p.w('<Import Project="%s" />', path.translate(build))
			end
		end)

	return result
end)

-- Hijack the userproject function to add ShowAllFiles by default in visual studio
-- All custom userproject properties can be injected here
-- premake.override(premake.vstudio.vc2010, "userProject", function(base)
--     base()
--     p.push('<PropertyGroup>')
--     p.w('<ShowAllFiles>true</ShowAllFiles>')
--     p.pop('</PropertyGroup>')
-- end)
