-- Set search paths for include()
premake.path = ("%s;%s/Source/Premake;%s/Source"):format(premake.path, _MAIN_SCRIPT_DIR, _MAIN_SCRIPT_DIR)

-- Register the root "Packages.lua" for future callers of `require "Packages"`
require "Packages"

-- Set search paths for require()
package.path = ("{}/Source/?.lua;{}/Source/Premake/?.lua"):gsub("{}", _MAIN_SCRIPT_DIR)

require "Action.Clean"
require "Action.Packages"
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
