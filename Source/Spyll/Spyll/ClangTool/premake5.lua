local Project = require "Project"
local Package = require "Package"
local Packages = require "Spyll.Packages"

group "Spyll"

project "Spyll.ClangTool"; do
	filename("%{prj.name}")

	language "C++"
	kind(premake.STATICLIB)

	Spyll.CommonCppSettings()

	Project.Files()

	links {
		"Spyll.Core",
		"ntdll",
		"version",
	}

	Package.Include(Packages.ClangTooling)

	filter "toolset:msc*"; do
		linkoptions {
			"/IGNORE:4006"
		}
	end
end
