premake.path = ("%s;%s/Source/Premake"):format(premake.path, _MAIN_SCRIPT_DIR)

require "Package"

require "Actions/Clean"
require "Overrides"

newoption {
	trigger = "workspace",
	description = "Generate only the selected workspace",
	value = "workspace",
	allowed = {
		{ "oyl3d", "Oyl3D Engine" },
		{ "spyll", "Oyl.Spyll Static Analysis Clang Plugin" }
	}
}

local function shouldGenerateWorkspace(wks)
	local workspaceOpt = _OPTIONS["workspace"]
	return not workspaceOpt or workspaceOpt:lower() == wks:lower()
end

if shouldGenerateWorkspace("Oyl3D") then
	include "Source/Oyl3D"
end

if shouldGenerateWorkspace("Spyll") then
	include "Source/Spyll"
end
