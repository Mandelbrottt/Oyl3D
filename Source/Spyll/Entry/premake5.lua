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

	Project.Files()
	
	includedirs {
		"%{prj.location}",
	}

	externalincludedirs {
		"%{wks.location}/",
	}

	defines {
		Project.InsideProjectMacro()
	}

	links {
		"Spyll.Tests",
		"Spyll.Core",
	}

	externalincludedirs(Packages.ClangTooling.Include)
end
