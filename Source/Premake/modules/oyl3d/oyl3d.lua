local p = premake

premake.modules.oyl3d = p.modules.oyl3d or {}

local m = p.modules.oyl3d

include("_preload.lua")
include("check.lua")
include("clean.lua")
include("packages.lua")
include("project.lua")
include("workspace.lua")

p.override(p.main, "preBake", function(base)
	base()
	m.preBake()
end)

function m.preBake()
	local global = p.api.scope.global
	for _, wks in ipairs(global.workspaces) do
		local cwd = os.getcwd()
		os.chdir(wks.basedir)

		workspace(wks.name); do
			m.generate.prepareWorkspace(wks)

			m.generate.generateWorkspaceProjects(wks)
			m.generate.generatePackageProjects(wks)

			for _, prj in ipairs(wks.projects) do
				local cwd = os.getcwd()
				os.chdir(prj.basedir)

				project(prj.name); do
					m.generate.applyProjectDefaults(prj)
					m.generate.applySharedToStaticLib(prj)

					m.generate.connectProjectLinks(prj)
				end
				os.chdir(cwd)
			end

			if p.action.isConfigurable() then
				m.generate.removeUnreferencedProjects(wks)
			end

			m.generate.generateCheckProject(wks)
		end
		os.chdir(cwd)
	end
end

return m
