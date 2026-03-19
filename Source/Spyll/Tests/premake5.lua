local Project = require "Project"
local Package = require "Package"
local Packages = require "Spyll.Packages"

group "Tests"

project "Spyll.Tests"; do
	filename("%{prj.name}")

	language "C++"
	kind(premake.CONSOLEAPP)

	pchheader "pch.h"
	pchsource "%{wks.location}/pch.cpp"
	files { "%{wks.location}/pch.cpp" }

	-- Ensure all test cases get loaded
	removeunreferencedcodedata "Off"

	Spyll.CommonCppSettings()

	Project.Files()

	includedirs {
		"%{prj.location}",
	}

	externalincludedirs {
		"%{wks.location}",
	}

	defines {
		Project.InsideProjectMacro()
	}

	links {
		"Spyll.Core",
	}

	debugdir "%{prj.location}/Targets/"

	externalincludedirs(Packages.ClangTooling.Include)

	Package.Include(Packages.GoogleTest)

	filter "files:Targets/**"; do
		excludefrombuild "On"
		enablepch "Off"
	end
end
