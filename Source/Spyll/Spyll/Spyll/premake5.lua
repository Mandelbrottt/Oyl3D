local Project = require "Project"
local Package = require "Package"
local Packages = require "Spyll.Packages"

group "Spyll"

project "Spyll.Core"; do
	filename("%{prj.name}")

	language "C++"
	kind(premake.STATICLIB)

	Spyll.CommonCppSettings()

	Project.Files()

	filter "toolset:msc*"; do
		linkoptions {
			"/IGNORE:4006"
		}
	end
end
