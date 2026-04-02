local Package = require "Package"
local Packages = require "Spyll.Packages"

group "Spyll/Spyll"

project "Spyll.Core"; do
	filename("%{prj.name}")

	language "C++"
	kind(premake.STATICLIB)

	Package.Include(Packages.NlohmannJson)

	filter "toolset:msc*"; do
		linkoptions {
			"/IGNORE:4006"
		}
	end
end
