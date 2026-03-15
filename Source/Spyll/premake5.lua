local Config = require "Config"
local Check = require "CheckProject"
local Package = require "Package"

---@type Package.Projects
local Packages = dofile "Packages.lua"

workspace "Spyll"; do
	location "./"
	filename("%{wks.name}")

	configurations {
		"Debug",
		"RelWithDebInfo",
		"Release"
	}

	platforms {
		premake.X86_64
	}

	startproject "Spyll"

	Check.GenerateProject {
		"--workspace=spyll"
	}

	---@param package? fun(package: Package.Project)
	local function commonCppSettings(package)
		cppdialect "C++17"
		staticruntime "Off"
		warnings "Extra"
		fatalwarnings { "All" }
		multiprocessorcompile "On"
		externalwarnings "Off"
		externalanglebrackets "On"
		usefullpaths "Off"

		targetdir(Config.TargetDir .. Config.OutputDir)
		debugdir(Config.TargetDir .. Config.OutputDir)
		objdir(Config.ObjectDir .. Config.OutputDir)
		implibdir(Config.LibraryDir .. Config.OutputDir)

		filter "configurations:Debug"; do
			optimize "Off"
			symbols "On"
		end

		filter "configurations:RelWithDebInfo"; do
			optimize "On"
			symbols "On"
		end

		filter "configurations:Release"; do
			optimize "Full"
			symbols "Off"
		end

		filter {}
	end

	Package.GeneratePackages {
		Packages = Packages,
		OnProject = commonCppSettings,
	}

	group "Spyll"
	project "Spyll"; do
		location "Spyll"
		kind(premake.CONSOLEAPP)

		includedirs {
			"%{prj.location}",
		}

		filename("%{prj.name}_" .. _ACTION)
		language "C++"

		commonCppSettings()

		Package.Include(Packages.ClangTooling)

		files {
			"./Spyll/**.cpp",
			"./Spyll/**.h",
			"./Spyll/**.hpp",
			"./Spyll/**.inl",
			"./Spyll/**.ixx",
		}

		files {
			"./Tests/**.cpp",
			"./Tests/**.h",
			"./Tests/**.hpp",
		}

		filter "files:Tests/**.cpp"; do
			excludefrombuild "On"
		end

		filter "system:windows"; do
			links {
				"ntdll",
				"version",
			}
		end

		links {
			"Premake"
		}
	end
end
