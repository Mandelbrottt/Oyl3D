local Project = require "Project"
local Workspace = require "Workspace"
local Package = require "Package"
local Config = require "Config"
local Check = require "CheckProject"

local p = premake
local api = p.api

api.register {
	name = "reflection",
	scope = "project",
	kind = "boolean"
}

api.register {
	name = "reflectionshowgenerated",
	scope = "project",
	kind = "boolean"
}

local Oyl3D = {}

Oyl3D.Name = "Oyl3D"
Oyl3D.ShortName = "Oyl"
Oyl3D.ShortNameUpper = string.upper(Oyl3D.ShortName)

local Private = {}

local prebake_overridden = false

local function OverridePreBake(wks)
	if prebake_overridden then
		return
	end

	premake.override(premake.main, "preBake", function(base)
		workspace(wks.name); do
			Private.ProcessWorkspace()
		end
		workspace("*")

		base()
	end)
end

--- @param name string
function Oyl3D.Workspace(name)
	workspace(name); do
		local wks = Workspace.Current()
		OverridePreBake(wks)
	end
end

--- @param name string
function Oyl3D.CppProject(name)
	project(name); do
		local prj = Project.Current()
		Private.PrepareDotIncludeDir(prj)

		language "C++"
		Oyl3D.DefaultCppSettings()

		Project.Files()
	end
end

local CppProjectDefaults = {}

function Oyl3D.DefaultCppSettings()
	CppProjectDefaults.LanguageSettings()
	CppProjectDefaults.FileSettings()
	CppProjectDefaults.ProjectSettings()
	CppProjectDefaults.ConfigurationSettings()
	CppProjectDefaults.ToolsetSettings()
	CppProjectDefaults.SystemSettings()
end

function CppProjectDefaults.LanguageSettings()
	cppdialect "C++20"
	cdialect "C11"
	warnings "Extra"
	fatalwarnings { "All" }
	rtti "On"
end

function CppProjectDefaults.ProjectSettings()
	targetdir(Config.BinariesDir)
	objdir(Config.ObjectDir)
	implibdir(Config.LibraryDir)
	debugdir(Config.BinariesDir)

	externalanglebrackets "On"
	externalwarnings "Off"
	floatingpoint "Fast"
	multiprocessorcompile "On"
	staticruntime "Off"
	stringpooling "On"

	includedirs {
		"%{prj.location}/..",
	}

	externalincludedirs {
		Config.SourceDir
	}

	defines {
		Project.CurrentAssemblyMacro(),
		Project.InsideProjectMacro(),
	}

	if os.isfile("pch.h") then
		local pchDir = path.join("%{wks.location}", "Pch")
		pchheader "pch.h"
		forceincludes { "pch.h" }
		pchsource(path.join(pchDir, "pch.cpp"))
		files { path.join(pchDir, "pch.cpp") }
		includedirs { pchDir }
		defines { string.format([[OYL_PCH_FILE="%s/pch.h"]], os.getcwd()) }
	end
end

function CppProjectDefaults.FileSettings()
	-- header files can be included across assembly boundaries, and so have to use project-agnostic includes
	-- FIXME: premake doesn't support per-file includedirs
	filter { "files:**.cpp" }; do
		includedirs {
			"%{prj.location}"
		}
	end

	filter { "files:**.hlsl" }; do
		excludefrombuild "On"
	end
	filter {}
end

function CppProjectDefaults.ConfigurationSettings()
	filter { "kind:StaticLib" }; do
		targetdir(Config.LibraryDir)
	end

	filter { "configurations:" .. Config.Configurations.Debug }; do
		defines { Oyl3D.ShortNameUpper .. "_DEBUG=1" }
		optimize "Off"
		runtime "Debug"
		symbols "On"
	end

	filter { "configurations:" .. Config.Configurations.Development }; do
		defines { Oyl3D.ShortNameUpper .. "_DEVELOPMENT=1" }
		optimize "On"
		runtime "Release"
		symbols "On"
	end

	filter { "configurations:" .. Config.Configurations.Profile }; do
		defines { Oyl3D.ShortNameUpper .. "_PROFILE=1", }
		optimize "On"
		runtime "Release"
		symbols "On"
	end

	filter { "configurations:" .. Config.Configurations.Distribution }; do
		defines { Oyl3D.ShortNameUpper .. "_DISTRIBUTION=1" }
		optimize "Full"
		runtime "Release"
		symbols "Off"
	end

	filter { "platforms:Editor" }; do
		defines { Oyl3D.ShortNameUpper .. "_EDITOR=1" }
	end
	filter {}
end

function CppProjectDefaults.ToolsetSettings()
	filter { "toolset:msc*" }; do
		disablewarnings {
			"4251", -- member needs dll-interface to be used by clients of class
			"4275", -- non dll-interface used as base for dll-interface
		}
	end

	filter { "toolset:clang" }; do
		floatingpoint "Default"
	end
	filter {}
end

function CppProjectDefaults.SystemSettings()
	filter { "system:windows" }; do
		architecture "x86_64"
	end

	filter { "system:not windows" }; do
		removefiles { "%{prj.location}/**_Windows*" }
	end
	filter {}
end

function Oyl3D.GenerateProjectsFromScripts()
	local wks = Workspace.Current()

	local location = wks.location
	if location then
		if not path.isabsolute(location) then
			location = path.normalize(path.join(wks.basedir, location))
		end
	else
		location = wks.basedir
	end

	local cwd = os.getcwd()
	os.chdir(location)

	-- Recurse through the workspace directory and include all premake scripts
	local scripts = os.matchfiles("**/premake5.lua")

	-- Remove all scripts that are included as part of the .Include dir
	local index = 1
	while index <= #scripts do
		local script = scripts[index]
		if string.contains(script, ".Include") then
			table.remove(scripts, index)
		else
			index = index + 1
		end
	end

	-- Invoke Project Scripts
	for _, script in ipairs(scripts) do
		local nProjects = #wks.projects
		Project.Script(script) -- Invoke the project script
		assert(
			#wks.projects == nProjects + 1,
			string.format("Project script %s must define one premake project!", script)
		)
	end

	os.chdir(cwd)
end

function Private.ProcessWorkspace()
	local wks = Workspace.Current()
	local workspaceProjects = Workspace.GetWorkspaceProjects(wks)

	for _, prj in ipairs(workspaceProjects) do
		project(prj.name); do
			if prj.kind == "SharedLib" then
				filter "configurations:not Editor"; do
					kind "StaticLib"
				end
				filter {}
			end

			Private.ConnectProjectLinks(prj)
			Private.AddSpyllPostBuildStep(prj)
		end
	end
	workspace(wks.name)
	Private.RemoveUnreferencedProjects(wks)

	-- Add CheckProject after all other processing is done
	if wks.checkproject then
		Check.GenerateProject()
	end
end

function Oyl3D.DotIncludeDir(prj)
	local location = prj.location
	if location then
		if not path.isabsolute(location) then
			location = path.normalize(path.join(prj.basedir, location))
		end
	else
		location = prj.basedir
	end
	return path.join(location, ".Include")
end

function Private.AddPathToDotIncludeDir(prj, dir, name)
	os.linkdir(
		dir,
		path.join(
			Oyl3D.DotIncludeDir(prj),
			name
		)
	)
end

function Private.PrepareDotIncludeDir(prj)
	local prjIncludeFolder = Oyl3D.DotIncludeDir(prj)
	if os.isdir(prjIncludeFolder) then
		os.rmdir(prjIncludeFolder)
	end

	-- Make .Include dir and mark the directory as hidden
	os.mkdir(prjIncludeFolder)
	if os.host() == premake.WINDOWS then
		os.executef("attrib +h %s /s /d", prjIncludeFolder)
	end

	removefiles { path.join(prjIncludeFolder, "**") }
	includedirs { prjIncludeFolder }
	externalincludedirs { prjIncludeFolder }
end

function Private.ConnectProjectLinks(prj)
	local wks = Workspace.Current()

	for _, link in ipairs(prj.links) do
		local linkprj = wks.projects[link]
		if linkprj then
			if linkprj._package then
				Package.Include(linkprj._package)
			else
				-- recurse links, add all children as links to parent projects for non-editor platform
				local function addLinksRecursive(link)
					links { link.links }
					libdirs { link.libdirs }
					for _, link in ipairs(link.links) do
						local prj = wks.projects[link]
						if prj then
							addLinksRecursive(prj)
						end
					end
				end
				filter "platforms:not *Editor*"; do
					addLinksRecursive(linkprj)
				end
				filter {}

				-- Add a symlink of the link project to the .Include dir
				assert(
					linkprj.basedir ~= wks.basedir,
					string.format(
						"Project \"%s\" shouldn't be in the same directory as workspace \"%s\"",
						linkprj.name,
						wks.name
					)
				)
				Private.AddPathToDotIncludeDir(prj, linkprj.basedir, path.getname(linkprj.basedir))
			end
		end
	end
end

function Private.AddSpyllPostBuildStep(prj)
	if not prj.reflection then
		return
	end

	dependson {
		"Oyl.Spyll",
	}

	-- Handle this logic in tokenized strings to ensure proper project filters are applied
	local spyllCommand = path.join(Config.BinariesDir, "Oyl.Spyll.exe")

	local function appendToCommand(str)
		-- Append trimmed string after space
		spyllCommand = spyllCommand .. " " .. str:match("^%s*(.-)%s*$")
	end

	appendToCommand '--assembly="%{cfg.buildtarget.basename}"'

	appendToCommand '--std="%{prj.cppdialect:lower()}"'

	appendToCommand '--include="%{table.concat(prj.includedirs, ";")}"'
	appendToCommand '--externalinclude="%{table.concat(prj.externalincludedirs, ";")}"'

	if premake.action.current() and premake.action.current().vstudio then
		appendToCommand '--externalinclude="$(IncludePath)"'
	end

	appendToCommand '--define="%{table.concat(prj.defines, ";")}"'

	-- Only include --pch arg if project has a pch
	appendToCommand '%{prj.pchheader and "--pch=" .. prj.pchheader or ""}'

	appendToCommand [[%{table.concat(
		table.translate(
				table.filter(
					prj.files,
					function(file) return path.hasextension(file, ".h") end
				),
				function(file) return '"' .. file .. '"' end
			),
			" "
		)}]]

	prebuildmessage("Executing " .. spyllCommand)

	prebuildcommands {
		"cd %{prj.location}",
		spyllCommand
	}

	files {
		path.join("%{wks.location}", "GeneratedInclude.cpp")
	}

	local generatedDir = ".Generated"
	Private.AddPathToDotIncludeDir(prj, path.join(prj.basedir, generatedDir), path.getname(generatedDir))

	local generatedFilesPattern = path.join("%{prj.location}", generatedDir, "**")

	if prj.reflectionshowgenerated then
		filter { "files:" .. generatedFilesPattern }; do
			excludefrombuild "On"
		end
		filter {}
	else
		removefiles {
			generatedFilesPattern
		}
	end
end

function Private.RemoveUnreferencedProjects(wks)
	-- Gather the set of projects referencing or being referenced by another project
	local dependSet = {}
	for _, prj in pairs(wks.projects) do
		if #prj.links > 0 then
			dependSet[prj.name] = true
		end
		for _, link in ipairs(prj.links) do
			dependSet[link] = true
		end
	end

	-- Remove all projects not referencing or being referenced by another project
	for index = #wks.projects, 1, -1 do
		local prj = wks.projects[index]
		if not dependSet[prj.name] then
			printf("Unreferenced package '%s' will not be Generated...", prj.name)
			table.remove(wks.projects, index)
		end
	end
	-- local index = 1
	-- while index <= #wks.projects do
	-- 	local prj = wks.projects[index]
	-- 	if dependSet[prj.name] then
	-- 		index = index + 1
	-- 	else
	-- 		table.remove(wks.projects, index)
	-- 	end
	-- end
end

return Oyl3D
