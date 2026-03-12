premake.path = ("%s;%s/Source/Premake"):format(premake.path, _MAIN_SCRIPT_DIR)

require "Dependencies"

local Package = require "Package"

require "Actions/Clean"
require "Overrides"

if not _OPTIONS["cc"] and premake.action.current() then
	premake.action.current().toolset = premake.CLANG
end

Package.SetupPackages()
Package.UpdatePackageCache()

newoption {
	trigger = "workspace",
	description = "Generate only the selected workspace",
	value = "workspace",
	allowed = {
		{ "oyl3d", "Oyl3D Engine" },
		{ "spyll", "Oyl.Spyll Static Analysis Clang Plugin"}
	}
}

local function shouldGenerateWorkspace(wks)
	local workspaceOpt = _OPTIONS["workspace"]
	return not workspaceOpt or workspaceOpt:lower() == wks:lower()
end

if shouldGenerateWorkspace("Oyl3D") then
	require "Source.Oyl3D.premake5"
end

-- if shouldGenerateWorkspace("Spyll") then
-- 	require "Source.Spyll.premake5"
-- end