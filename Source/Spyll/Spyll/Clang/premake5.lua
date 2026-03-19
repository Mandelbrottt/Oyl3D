local Project = require "Project"
local Package = require "Package"
local Packages = require "Spyll.Packages"

group "Spyll"

project "Spyll.Core.Clang"; do
	filename("%{prj.name}")

	language "C++"
	kind(premake.STATICLIB)

	pchsource "pch.cpp"
	pchheader "pch.h"

	Spyll.CommonCppSettings()

	Project.Files()

	includedirs {
		"%{prj.location}",
	}

	defines {
		Project.InsideProjectMacro()
	}

	links {
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
