local p = premake

local oyl3d = p.modules.oyl3d
oyl3d.postAction = oyl3d.postAction or {}

local m = oyl3d.postAction
local private = {}

function m.onWorkspacePrepare(wks)
	local wksDotIncludeDir = oyl3d.wksDotIncludeDir(wks)

	if os.isdir(wksDotIncludeDir) then
		os.rmdir(wksDotIncludeDir)
	end
end

return m
