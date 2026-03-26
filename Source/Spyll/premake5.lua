local Check = require "CheckProject"
local Config = require "Config"
local Package = require "Package"
local Project = require "Project"

---@type WorkspacePackage.List
local Packages = require "Spyll.Packages"
Package.InitWorkspacePackages(Packages)

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
	debugdir(Config.TargetDir)
	objdir(Config.ObjectDir)
	implibdir(Config.LibraryDir)

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
		targetdir(Config.LibraryDir .. Config.OutputDir)
	end

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

	filter{}
	-- Currently only include the release clang tooling in the package
	-- That compiles against release runtime STL, so all other projects have to as well to
	-- maintain ABI
	Packages.ClangTooling:OnDepend()
	filter {}
end

workspace "Spyll"; do
	filename("%{wks.name}")

	configurations(table.keys(Config.Configurations))
	configmap {
		[Config.Configurations.Profile] = Config.Configurations.Development,
	}

	platforms {
		"x64"
	}

	for _, script in ipairs(os.matchfiles("**/premake5.lua")) do
		script = path.getabsolute(script)
		Project.Script(script)
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
