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
	
	defines {
		Project.InsideProjectMacro()
	}

	links {
		"Spyll.ClangTool",
	}

	dependson {
		"Spyll.Tests"
	}

	externalincludedirs(Packages.ClangTooling.Include)
end
