local Check = require "CheckProject"
local Config = require "Config"
local Packages = require "Packages"

local p = premake

p.modules.oyl3d = p.modules.oyl3d or {}

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

function m.files()
	
end

function m.preBake()
	local global = p.api.scope.global
	for _, wks in ipairs(global.workspaces) do
		local cwd = os.getcwd()
		os.chdir(wks.basedir)

		workspace(wks.name); do
			m.workspace.prepareWorkspace(wks)

			m.workspace.generateWorkspaceProjects(wks)
			m.workspace.generatePackageProjects(wks)

			for _, prj in ipairs(wks.projects) do
				local cwd = os.getcwd()
				os.chdir(prj.basedir)

				project(prj.name); do
					m.project.applyProjectDefaults(prj)
					m.project.applySharedToStaticLib(prj)

					m.project.connectProjectLinks(prj)
				end
				os.chdir(cwd)
			end

			m.workspace.removeUnreferencedProjects(wks)

			m.check.generateCheckProject(wks)
		end
		os.chdir(cwd)
	end
end

return m
