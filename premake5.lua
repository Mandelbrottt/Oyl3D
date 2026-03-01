premake.path = ("%s;%s/Source/Premake"):format(premake.path, _MAIN_SCRIPT_DIR)

require "Dependencies"

local Package = require "Package"
local Library = require "Library"

require "Actions/Clean"
require "Overrides"

if not _OPTIONS["cc"] and premake.action.current() then
	premake.action.current().toolset = premake.CLANG
end

Package.SetupPackages()
Package.UpdatePackageCache()

Library.SetupLibraries()

newoption {
	trigger = "workspace",
	description = "Generate only the selected workspace",
	value = "workspace",
	allowed = {
		{ "oyl3d", "Oyl3D Engine" },
		{ "spyll", "Oyl.Spyll Static Analysis Clang Plugin"}
	}
}

local generateWorkspaceOpt = _OPTIONS["workspace"]
if generateWorkspaceOpt then
	generateWorkspaceOpt = generateWorkspaceOpt:lower()
end

local function shouldGenerateWorkspace(wks)
	local workspaceOpt = _OPTIONS["workspace"]
	return not workspaceOpt or workspaceOpt:lower() == wks:lower()
end

if shouldGenerateWorkspace("Oyl3D") then
	require "Source.Oyl3D.premake5"
end

