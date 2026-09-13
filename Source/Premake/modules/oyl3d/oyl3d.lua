local p = premake

p.modules.oyl3d = p.modules.oyl3d or {}
p.modules.oyl3d._VERSION = "0.0.1"

local m = p.modules.oyl3d

p.override(p.main, "preBake", function(base)
	local global = p.api.scope.global
	for _, wks in ipairs(global.workspaces) do
		for _, prj in ipairs(wks.projects) do
			
		end
	end

	base()
end)

include("_preload.lua")

return m