local Check = require "CheckProject"
local Config = require "Config"
local Package = require "Package"
local Project = require "Project"

---@type WorkspacePackage.List
local Packages = require "Spyll.Packages"

Spyll = {}

---@param package? WorkspacePackage
function Spyll.CommonCppSettings(package)
	cppdialect "C++17"
	staticruntime "Off"
	warnings "Extra"
	fatalwarnings { "All" }
	multiprocessorcompile "On"
	externalwarnings "Off"
	externalanglebrackets "On"

	targetdir(Config.TargetDir)
	objdir(Config.ObjectDir)
	implibdir(Config.LibraryDir)
	debugdir(Config.TargetDir)

	Project.Files()

	if os.isfile("pch.h") then
		pchheader "pch.h"
		forceincludes { "pch.h" }
		pchsource "%{wks.location}/pch.cpp"
		files { "%{wks.location}/pch.cpp" }
	end

	includedirs {
		"%{prj.location}",
		"%{wks.location}",
	}

	defines {
		Project.InsideProjectMacro()
	}

	filter "kind:StaticLib"; do
		targetdir(Config.LibraryDir)
	end

	filter(("configurations:%s"):format(Config.Configurations.Debug)); do
		optimize "Off"
		symbols "On"
	end

	filter(("configurations:%s"):format(Config.Configurations.Development)); do
		optimize "On"
		symbols "On"
	end

	filter(("configurations:%s"):format(Config.Configurations.Distribution)); do
		optimize "Full"
		symbols "Off"
	end

	filter {}
	-- Currently only include the release clang tooling in the package
	-- That compiles against release runtime STL, so all other projects have to as well to
	-- maintain ABI
	Packages.ClangTooling:OnDepend()
	filter {}
end

workspace "Spyll"; do
	filename("%{wks.name}")

	configurations {
		Config.Configurations.Debug,
		Config.Configurations.Development,
		Config.Configurations.Profile,
		Config.Configurations.Distribution,
	}
	configmap {
		[Config.Configurations.Profile] = Config.Configurations.Development,
	}

	platforms {
		"x64"
	}

	for _, script in ipairs(os.matchfiles("**/premake5.lua")) do
		script = path.getabsolute(script)
		Project.Script(script)

		-- Scope the project
		project()

		Project.PrependBlocks(function()
			-- cd into script directory and run common project settings
			local cwd = os.getcwd()
			os.chdir(path.getdirectory(script))

			Spyll.CommonCppSettings()

			os.chdir(cwd)
		end)

		project "*"
	end

	Package.GenerateWorkspacePackageProjects {
		Packages = Packages,
		OnProject = Spyll.CommonCppSettings,
	}

	Check.GenerateProject {
		"--workspace=spyll"
	}
end
