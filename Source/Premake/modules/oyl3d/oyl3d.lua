local p = premake

premake.modules.oyl3d = p.modules.oyl3d or {}

local m = p.modules.oyl3d
m.overrides = m.overrides or {}
local private = {}

include("_preload.lua")

include("oyl3d_prebake.lua")
include("oyl3d_postbake.lua")
include("oyl3d_postaction.lua")

include("check.lua")
include("clean.lua")
include("packages.lua")
include("project.lua")
include("workspace.lua")

m.elements = {}

--- PreBake ---
m.elements.preBake = {}

m.elements.preBake.onWorkspacePreProcess = function(wks)
	return {
		m.preBake.onWorkspaceModuleProjects,
		m.preBake.onWorkspacePackageProjects,
	}
end

m.elements.preBake.onProjectPreProcessCommon = function(prj)
	return {
		m.preBake.onProjectApplyDefaults,
		m.preBake.onProjectApplySharedToStaticLib,
	}
end

m.elements.preBake.onProjectPreProcessModule = function(prj)
	return {
		m.preBake.onProjectDefineModuleMacros,
		m.preBake.onProjectModuleProjectUsages,
		m.preBake.onProjectAddReflectionBuildAction,
	}
end

m.elements.preBake.onProjectPreProcessPackage = function(prj)
	return {}
end

m.elements.preBake.onProjectPostProcess = function(prj)
	return {
		m.preBake.onProjectConnectProjectLinks,
		-- m.preBake.onProjectRemoveLinksToNoneKindProjects,
		m.preBake.onProjectRemoveStaticLibLinks,
	}
end

m.elements.preBake.onWorkspacePostProcess = function(wks)
	return {
		m.preBake.onWorkspaceRemoveNonProjectPackages,
		-- m.preBake.onWorkspaceRemoveUnreferencedProjects,
		m.check.generateCheckProject,
	}
end

p.override(p.main, "preBake", function(base)
	m.overrides.preBake()
	base()
end)

function m.overrides.preBake()
	local global = p.api.scope.global
	for _, wks in ipairs(global.workspaces) do
		local cwd = os.getcwd()
		os.chdir(wks.basedir)
		workspace(wks.name); do
			-- Generate the workspace
			for func in private.eachfunc(m.elements.preBake.onWorkspacePreProcess, wks) do
				workspace(wks.name)
				func(wks)
			end

			-- Generate information for each project
			private.callOnEachProjectInWorkspace(wks, function(prj)
				for func in private.eachfunc(m.elements.preBake.onProjectPreProcessCommon, prj) do
					project(prj.name)
					func(prj)
				end
				if prj._package then
					for func in private.eachfunc(m.elements.preBake.onProjectPreProcessPackage, prj) do
						project(prj.name)
						func(prj)
					end
				else
					for func in private.eachfunc(m.elements.preBake.onProjectPreProcessModule, prj) do
						project(prj.name)
						func(prj)
					end
					project(prj.name)
					includedirs {
						m.prjDotIncludeDir(prj)
					}
				end
			end)

			private.callOnEachProjectInWorkspace(wks, function(prj)
				for func in private.eachfunc(m.elements.preBake.onProjectPostProcess, prj) do
					project(prj.name)
					func(prj)
				end
			end)

			-- The workspace is now fully generated
			for func in private.eachfunc(m.elements.preBake.onWorkspacePostProcess, wks) do
				workspace(wks.name)
				func(wks)
			end
		end
		os.chdir(cwd)
	end
end

--- PreBake ---

--- PostBake ---
m.elements.postBake = {}

m.elements.postBake.onWorkspacePreProcess = function(wks)
	return {}
end

m.elements.postBake.onProjectPreProcess = function(wks)
	return {}
end

m.elements.postBake.onConfigPreProcess = function(cfg)
	return {
		m.postBake.onConfigConnectProjectLinks,
	}
end

m.elements.postBake.onConfigPostProcess = function(cfg)
	return {
		m.postBake.onConfigDefineModuleDependencies,
	}
end

m.elements.postBake.onProjectPostProcess = function(wks)
	return {}
end

m.elements.postBake.onWorkspacePostProcess = function(wks)
	return {}
end

p.override(p.main, "postBake", function(base)
	m.overrides.postBake()
	base()
end)

function m.overrides.postBake()
	if not p.action.isConfigurable() then
		return
	end

	local global = p.api.scope.global
	for _, wks in ipairs(global.workspaces) do
		local cwd = os.getcwd()
		os.chdir(wks.basedir)
		do
			p.callArray(m.elements.postBake.onWorkspacePreProcess, wks)

			-- PreProcess
			private.callOnEachProjectInWorkspace(wks, function(prj)
				p.callArray(m.elements.postBake.onProjectPreProcess, prj)

				for cfg in p.project.eachconfig(prj) do
					p.callArray(m.elements.postBake.onConfigPreProcess, cfg)
				end
			end)

			-- PostProcess
			private.callOnEachProjectInWorkspace(wks, function(prj)
				for cfg in p.project.eachconfig(prj) do
					p.callArray(m.elements.postBake.onConfigPostProcess, cfg)
				end

				p.callArray(m.elements.postBake.onProjectPostProcess, prj)
			end)

			p.callArray(m.elements.postBake.onWorkspacePostProcess, wks)
		end
		os.chdir(cwd)
	end
end

--- PostBake ---

--- PostAction ---
m.elements.postAction = {}

m.elements.postAction.onWorkspacePreProcess = function(wks)
	return {
		m.postAction.onWorkspacePrepare,
	}
end

m.elements.postAction.onProjectPreProcess = function(prj)
	return {
		m.postAction.onProjectCreateLinkDotIncludeDirs,
	}
end

m.elements.postAction.onConfigPreProcess = function(prj)
	return {}
end

m.elements.postAction.onConfigPostProcess = function(prj)
	return {}
end

m.elements.postAction.onProjectPostProcess = function(prj)
	return {}
end

m.elements.postAction.onWorkspacePostProcess = function(wks)
	return {}
end

p.override(p.main, "postAction", function(base)
	m.overrides.postAction()
	base()
end)

function m.overrides.postAction()
	local global = p.api.scope.global
	for _, wks in ipairs(global.workspaces) do
		local cwd = os.getcwd()
		os.chdir(wks.basedir)
		do
			p.callArray(m.elements.postAction.onWorkspacePreProcess, wks)

			-- PreProcess
			private.callOnEachProjectInWorkspace(wks, function(prj)
				p.callArray(m.elements.postAction.onProjectPreProcess, prj)

				for cfg in p.project.eachconfig(prj) do
					p.callArray(m.elements.postAction.onConfigPreProcess, cfg)
				end
			end)

			-- PostProcess
			private.callOnEachProjectInWorkspace(wks, function(prj)
				for cfg in p.project.eachconfig(prj) do
					p.callArray(m.elements.postAction.onConfigPostProcess, cfg)
				end

				p.callArray(m.elements.postAction.onProjectPostProcess, prj)
			end)

			p.callArray(m.elements.postAction.onWorkspacePostProcess, wks)
		end
		os.chdir(cwd)
	end
end

--- PostAction ---

function private.callOnEachProjectInWorkspace(wks, func)
	for _, prj in ipairs(wks.projects) do
		local cwd = os.getcwd()
		os.chdir(prj.basedir)
		project(prj.name); do
			func(prj)
		end
		os.chdir(cwd)
	end
end

function private.eachfunc(funcs, ...)
	if type(funcs) == "function" then
		funcs = funcs(...)
	end
	local i = 0
	local n = #funcs
	return function()
		i = i + 1
		if i <= n then return funcs[i] end
	end
end

function private.perCallableElementCallback(arr, callback, ...)
	if type(arr) == "function" then
		arr = arr(...)
	end
	return table.translate(
		arr,
		function(func)
			return function(...)
				callback()
				func(...)
			end
		end
	)
end

return m
