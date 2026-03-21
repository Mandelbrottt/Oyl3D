local Project = require "Project"
local Package = require "Package"
local Packages = require "Spyll.Packages"

group "Spyll/Tests"

project "Spyll.Tests"; do
	filename("%{prj.name}")

	language "C++"
	kind(premake.CONSOLEAPP)

	Spyll.CommonCppSettings()
	
	-- Ensure all test cases get loaded
	removeunreferencedcodedata "Off"

	Project.Files()

	links {
		"Spyll.ClangTool",
	}

	debugdir "%{prj.location}/Targets/"

	externalincludedirs(Packages.ClangTooling.Include)

	Package.Include(Packages.GoogleTest)

	filter "files:Targets/**"; do
		excludefrombuild "On"
		enablepch "Off"
	end
end
