local Project = require "Project"
local Package = require "Package"
local Packages = require "Spyll.Packages"

group "Spyll/Tests"

project "Spyll.Tests"; do
	filename("%{prj.name}")

	language "C++"
	kind(premake.CONSOLEAPP)

	-- Ensure all test cases get loaded
	removeunreferencedcodedata "Off"

	links {
		"Spyll.ClangTool",
	}

	debugdir "%{prj.location}/Targets/"

	externalincludedirs(Packages.ClangTooling.Include)

	Package.Include(Packages.GoogleTest)
	Package.Include(Packages.NlohmannJson)

	filter "files:Targets/**"; do
		excludefrombuild "On"
		enablepch "Off"
	end
end
