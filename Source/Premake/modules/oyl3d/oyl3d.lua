local p = premake

premake.modules.oyl3d = p.modules.oyl3d or {}

local m = p.modules.oyl3d

include("_preload.lua")
include("check.lua")
include("clean.lua")
include("packages.lua")
include("project.lua")
include("workspace.lua")

m.elements = {}

m.elements.workspaceGenerate = function(wks)
	return {
		m.generate.generateWorkspaceProjects,
		m.generate.generatePackageProjects
	}
end

m.elements.projectGenerate = function(prj)
	return {
		m.generate.applyProjectDefaults,
		m.generate.applySharedToStaticLib,
	}
end

m.elements.workspaceProject = function(prj)
	return {
		m.generate.generateReflectionInfo
	}
end

m.elements.packageProject = function(prj)
	return {}
end

m.elements.projectsComplete = function(prj)
	return {
		m.generate.connectProjectLinks,
		m.generate.removeStaticLibLinks,
	}
end

m.elements.workspaceComplete = function(wks)
	return {
		m.generate.removeNonProjectPackages,
		-- m.generate.removeUnreferencedProjects,
		m.generate.generateCheckProject,
	}
end

m.elements.workspaceAction = function(wks)
	return {
		m.generate.prepareWorkspace
	}
end

m.elements.projectAction = function(prj)
	return {
		m.generate.createProjectLinkDirs
	}
end

p.override(p.main, "preBake", function(base)
	m.preBake()
	base()
end)

function m.preBake()
	local global = p.api.scope.global
	for _, wks in ipairs(global.workspaces) do
		local cwd = os.getcwd()
		os.chdir(wks.basedir)
		workspace(wks.name); do
			-- Generate the workspace
			p.callArray(m.elements.workspaceGenerate, wks)

			-- Generate information for each project
			for _, prj in ipairs(wks.projects) do
				local cwd = os.getcwd()
				os.chdir(prj.basedir)
				project(prj.name); do
					p.callArray(m.elements.projectGenerate, prj)
				end
				os.chdir(cwd)
			end

			-- Per project configuration that depends on other projects' configurations
			for _, prj in ipairs(wks.projects) do
				local cwd = os.getcwd()
				os.chdir(prj.basedir)
				project(prj.name); do
					p.callArray(m.elements.projectsComplete, prj)
				end
				os.chdir(cwd)
			end

			-- The workspace is now fully generated
			workspace(wks.name)
			p.callArray(m.elements.workspaceComplete, wks)
		end
		os.chdir(cwd)
	end
end

p.override(p.main, "postAction", function(base)
	m.postAction()
	base()
end)

function m.postAction()
	local global = p.api.scope.global
	for _, wks in ipairs(global.workspaces) do
		local cwd = os.getcwd()
		os.chdir(wks.basedir)
		workspace(wks.name); do
			p.callArray(m.elements.workspaceAction, wks)

			for _, prj in ipairs(wks.projects) do
				local cwd = os.getcwd()
				os.chdir(prj.basedir)
				project(prj.name); do
					p.callArray(m.elements.projectAction, prj)
				end
				os.chdir(cwd)
			end
		end
		os.chdir(cwd)
	end
end

return m
