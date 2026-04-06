local Package = require "Package"
local Packages = require "Spyll.Packages"

group "Spyll/Spyll"

project "Spyll.Core"; do
	filename("%{prj.name}")

	language "C++"
	kind "StaticLib"

	Package.Include(Packages.NlohmannJson)

	Packages.ClangTooling:OnDepend();

	filter "toolset:msc*"; do
		linkoptions {
			"/IGNORE:4006"
		}
	end
end
