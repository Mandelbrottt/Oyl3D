local Project = require "Project"
local Package = require "Package"
local Packages = require "Spyll.Packages"

group "Tests"

project "Spyll.Tests"; do
	filename("%{prj.name}")

	language "C++"
	kind(premake.CONSOLEAPP)

	pchsource "pch.cpp"
	pchheader "pch.h"

	-- Ensure all test cases get loaded
	removeunreferencedcodedata "Off"

	Spyll.CommonCppSettings()

	files {
		"%{prj.location}/**.cpp",
		"%{prj.location}/**.h",
		"%{prj.location}/**.hpp",
		"%{prj.location}/**.inl",
		"%{prj.location}/**.ixx",
	}

	externalincludedirs {
		"%{prj.location}",
	}

	externalincludedirs {
		"%{wks.location}/Spyll/",
	}

	defines {
		Project.InsideProjectMacro()
	}

	links {
		"Spyll.Core"
	}

	debugdir "%{prj.location}/Targets/"

	externalincludedirs(Packages.ClangTooling.Include)

	Package.Include(Packages.GoogleTest)

	filter "files:Targets/**"; do
		excludefrombuild "On"
		enablepch "Off"
	end
end
