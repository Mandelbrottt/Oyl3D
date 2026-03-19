local Project = require "Project"
local Package = require "Package"
local Packages = require "Spyll.Packages"

group "Executables"

project "Spyll.Entry"; do
	filename "%{prj.name}"

	language "C++"
	kind(premake.CONSOLEAPP)

	targetname "oyl-spyll"

	Spyll.CommonCppSettings()

	files {
		"%{prj.location}/**.cpp",
		"%{prj.location}/**.h",
		"%{prj.location}/**.hpp",
		"%{prj.location}/**.inl",
		"%{prj.location}/**.ixx",
	}

	includedirs {
		"%{prj.location}",
	}

	defines {
		Project.InsideProjectMacro()
	}

	externalincludedirs {
		"%{wks.location}/Spyll/",
	}

	links {
		"Spyll.Tests",
		"Spyll.Core",
	}

	externalincludedirs(Packages.ClangTooling.Include)
end
