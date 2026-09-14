local p = premake

local oyl3d = p.modules.oyl3d

oyl3d.actions = oyl3d.actions or {}
oyl3d.actions.packages = oyl3d.actions.packages or {}

local m = oyl3d.actions.packages
local private = {}

oyl3d.actions.newaction {
	trigger = "packages",
	description = "Fetch all packages defined in Packages.lua files",

	options = function()
		oyl3d.packages.options()
	end,
	execute = function()
		oyl3d.packages.execute()
	end,
}

return m