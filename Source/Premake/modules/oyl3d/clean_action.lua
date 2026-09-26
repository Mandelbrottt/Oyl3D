local p = premake

local oyl3d = p.modules.oyl3d

oyl3d.actions = oyl3d.actions or {}
oyl3d.actions.clean = oyl3d.actions.clean or {}

local m = oyl3d.actions.clean

oyl3d.actions.newaction {
	trigger = "clean",
	description = "Deletes the build directory and all project files",

	options = function()
		newoption {
			trigger     = "packages",
			description = "Clean the package cache",
		}
	end,
	execute = function()
		oyl3d.clean.execute()
	end,
	onWorkspace = function(wks)
		oyl3d.clean.onWorkspace(wks)
	end,
	onProject = function(prj)
		oyl3d.clean.onProject(prj)
	end,
}

return m
