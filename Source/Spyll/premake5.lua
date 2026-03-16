local Check = require "CheckProject"
local Config = require "Config"
local Package = require "Package"

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

	targetdir(Config.TargetDir .. Config.OutputDir)
	debugdir(Config.TargetDir .. Config.OutputDir)
	objdir(Config.ObjectDir .. Config.OutputDir)
	implibdir(Config.LibraryDir .. Config.OutputDir)

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
	Packages.ClangTooling:OnDepend()
	filter {}
end

workspace "Spyll"; do
	filename("%{wks.name}")

	configurations {
		"Debug",
		"RelWithDebInfo",
		"Release"
	}

	platforms {
		"x64"
	}

	include "Spyll/Entry"; project "*"
	include "Spyll/Spyll"; project "*"
	include "Spyll/Tests"; project "*"

	Package.GenerateWorkspacePackageProjects {
		Packages = Packages,
		OnProject = Spyll.CommonCppSettings,
	}

	Check.GenerateProject {
		"--workspace=spyll"
	}
end
