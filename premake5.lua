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

require "Source.Oyl3D.premake5"