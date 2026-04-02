local Project = require "Project"
local Package = require "Package"
local Packages = require "Spyll.Packages"

group "Spyll/Executables"

startproject "Spyll.Entry"

project "Spyll.Entry"; do
	filename "%{prj.name}"

	language "C++"
	kind "ConsoleApp"

	targetname "spyll-cli"

	debugdir(path.join("%{wks.location}", "Tests", "Targets"))

	local compileFlagsFile = "compile_flags.txt"
	if (os.isfile(compileFlagsFile)) then
		files { compileFlagsFile }
		debugargs {
			"-p",
			path.join("%{!prj.location}", compileFlagsFile)
		}
	end

	links {
		"Spyll.ClangTool",
	}

	Package.Include(Packages.NlohmannJson)

	externalincludedirs(Packages.ClangTooling.Include)
end
