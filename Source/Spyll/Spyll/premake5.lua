local Project = require "Project"
local Package = require "Package"
local Packages = require "Spyll.Packages"

group "Spyll"

project "Spyll.Core"; do
	filename("%{prj.name}")

	language "C++"
	kind(premake.STATICLIB)

	pchheader "pch.h"
	forceincludes { "pch.h" }
	pchsource "%{wks.location}/pch.cpp"
	files { "%{wks.location}/pch.cpp" }

	Spyll.CommonCppSettings()

	Project.Files()

	includedirs {
		"%{prj.location}",
	}

	defines {
		Project.InsideProjectMacro()
	}

	links {
		"Spyll.Core.Clang"
	}

	externalincludedirs(Packages.ClangTooling.Include)

	filter "toolset:msc*"; do
		linkoptions {
			"/IGNORE:4006"
		}
	end
end
