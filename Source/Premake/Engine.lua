local Config = require "Config"
local Package = require "Package"
local Project = require "Project"

local Engine = {}

Engine.Name = "Oyl3D"
Engine.ShortName = "Oyl"

local function CommonProjectSettings()
	local prj = Project.CurrentProject()

	if not prj.language then language "C++" end
	if not prj.kind then kind "None" end

	if prj.language == premake.CPP then
		Engine.CommonCppSettings()
	else
		error(string.format("Invalid language \"%s\" in project \"%s\"", prj.language, prj.name))
	end

	filter("configurations:" .. Config.Configurations.Debug); do
		defines { string.upper(Engine.ShortName) .. "_DEBUG=1" }
	end
	filter("configurations:" .. Config.Configurations.Development); do
		defines { string.upper(Engine.ShortName) .. "_DEVELOPMENT=1" }
	end
	filter("configurations:" .. Config.Configurations.Profile); do
		defines { string.upper(Engine.ShortName) .. "_PROFILE=1", }
	end
	filter("configurations:" .. Config.Configurations.Distribution); do
		defines { string.upper(Engine.ShortName) .. "_DISTRIBUTION=1" }
	end
	filter "platforms:Editor"; do
		defines { string.upper(Engine.ShortName) .. "_EDITOR=1" }
	end
	filter {}
end

---@param script string
function Engine.SetupProjectFromScript(script)
	-- Reset back to workspace scope to ensure a clean slate for the callee
	---@type any
	---@diagnostic disable-next-line: missing-parameter
	local wks = workspace()

	do
		local nProjects = #wks.projects
		Project.Script(script) -- Invoke the project script
		assert(
			#wks.projects == nProjects + 1,
			string.format("Project script %s must define one premake project!", script)
		)
	end

	local cwd = os.getcwd()
	os.chdir(path.getdirectory(script))

	-- Get the project that was just created
	---@type any
	---@diagnostic disable-next-line: missing-parameter
	local prj = assert(project(), "No engine project in scope!")

	Project.PrependBlocks(function()
		Project.Files()
		
		CommonProjectSettings()
	end)

	os.chdir(cwd)

	return prj
end

local function RemoveUnreferencedProjects(wks, engineProjects)
	-- wks.projects is an array, make it a set for O(n) checking
	local projectSet = {}
	for _, prj in ipairs(wks.projects) do
		projectSet[prj.name] = prj
	end

	-- Gather the set of projects referenced by engine projects
	local dependSet = {}
	for _, prj in pairs(engineProjects) do
		dependSet[prj.name] = prj
		for _, link in ipairs(prj.links) do
			dependSet[link] = projectSet[link]
		end
	end

	-- Remove all projects not directly referenced by an engine project
	local index = 1
	while index <= #wks.projects do
		local prj = wks.projects[index]
		if dependSet[prj.name] then
			index = index + 1
		else
			printf("Unreferenced package '%s' will not be Generated...", prj.name)
			table.remove(wks.projects, index)
		end
	end
end

---@class Engine.GenerateProjectParams
---@field Packages? WorkspacePackage.List

---@param params? Engine.GenerateProjectParams
function Engine.GenerateProjects(params)
	params = params or {}

	-- Generate package projects before engine projects to guarantee they will be ready when dependencies
	-- are realized
	if params.Packages then
		Package.GenerateWorkspacePackageProjects {
			Packages = params.Packages,
			OnProject = function(package)
				if package.Language == premake.CPP or package.Language == premake.C then
					Engine.CommonCppSettings()
				else
					error(string.format(
						"Language \"%s\" in package \"%s\" not currently supported!",
						package.Language,
						package.Name
					))
				end
			end
		}
	end

	local engineProjects = {}

	-- Recurse through the source directory and include all premake scripts
	local scripts = os.matchfiles("**/premake5.lua")
	for _, script in ipairs(scripts) do
		local prj = Engine.SetupProjectFromScript(script)
		engineProjects[prj.name] = prj
	end

	-- Iterate all workspace projects
	--     Hook up their dependencies
	--     Remove projects that aren't referenced
	for _, prj in pairs(engineProjects) do
		project(prj.name)
		for _, link in ipairs(prj.links) do
			local proj = engineProjects[link]
			if proj then
				includedirs(proj.basedir)
				externalincludedirs(proj.basedir)
			end

			local package = params.Packages[link]
			if package then
				Package.Include(package)
			end
		end
	end

	---@type any
	---@diagnostic disable-next-line: missing-parameter
	local wks = workspace()
	RemoveUnreferencedProjects(wks, engineProjects)
end

function Engine.CommonCppSettings()
	local prj = Project.CurrentProject()

	staticruntime "Off"
	floatingpoint "Fast"
	rtti "On"
	stringpooling "On"
	warnings "Extra"
	fatalwarnings { "All" }
	multiprocessorcompile "On"
	externalwarnings "Off"
	externalanglebrackets "On"
	cppdialect "C++17"
	cdialect "C11"

	targetdir(Config.BinariesDir)
	objdir(Config.ObjectDir)
	implibdir(Config.LibraryDir)
	debugdir(Config.BinariesDir)

	includedirs {
		"%{prj.location}",
	}

	externalincludedirs {
		Config.SourceDir
	}

	defines {
		Project.CurrentAssemblyMacro(),
		Project.InsideProjectMacro(),
	}

	if os.isfile("pch.h") then
		pchheader "pch.h"
		forceincludes { "pch.h" }
		pchsource "%{wks.location}/pch.cpp"
		files { "%{wks.location}/pch.cpp" }
	end

	filter "system:not windows"; do
		removefiles { "%{prj.location}/**_Windows*" }
	end

	-- header files can be included across assembly boundaries, and so have to use project-agnostic includes
	-- FIXME: premake doesn't support per-file includedirs
	filter "files:**.cpp"; do
		includedirs {
			prj.basedir,
		}
	end

	filter "kind:StaticLib"; do
		targetdir(Config.LibraryDir)
	end

	filter "toolset:msc*"; do
		disablewarnings {
			"4251", -- member needs dll-interface to be used by clients of class
			"4275", -- non dll-interface used as base for dll-interface
		}
	end

	filter "toolset:clang"; do
		floatingpoint "Default"
	end

	filter "system:windows"; do
		architecture "x86_64"
	end

	filter("configurations:" .. Config.Configurations.Debug); do
		optimize "Off"
		runtime "Debug"
		symbols "On"
	end

	filter("configurations:" .. Config.Configurations.Development); do
		optimize "On"
		runtime "Release"
		symbols "On"
	end

	filter("configurations:" .. Config.Configurations.Profile); do
		optimize "On"
		runtime "Release"
		symbols "On"
	end

	filter("configurations:" .. Config.Configurations.Distribution); do
		optimize "Full"
		runtime "Release"
		symbols "Off"
	end

	local prj = Project.CurrentProject()
	if prj.kind == premake.SHAREDLIB then
		filter "platforms:not *Editor*"; do
			kind "StaticLib"
		end
	end

	filter {}
end

return Engine
