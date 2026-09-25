local Config = require "Config"

local p = premake

local oyl3d = p.modules.oyl3d
oyl3d.workspace = oyl3d.workspace or {}
oyl3d.generate = oyl3d.generate or {}

local m = oyl3d.workspace
local generate = oyl3d.generate

local private = {}

local package_fetch_table = require(_MAIN_SCRIPT_DIR .. ".Packages")
local package_fetch_file = path.join(_MAIN_SCRIPT_DIR, "Packages.lua")

function generate.wksDotIncludeDir(wks)
	return path.join(wks.basedir, ".Include")
end

function generate.createWksDotIncludeDir(wks)
	local dotIncludeDir = generate.wksDotIncludeDir(wks)

	if os.isdir(dotIncludeDir) then
		return
	end

	-- Make .Include dir and mark the directory as hidden
	os.mkdir(dotIncludeDir)
	if os.host() == premake.WINDOWS then
		os.executef("attrib +h %s /s /d", dotIncludeDir)
	end
end

function generate.prepareWorkspace(wks)
	local wksDotIncludeDir = generate.wksDotIncludeDir(wks)

	if os.isdir(wksDotIncludeDir) then
		os.rmdir(wksDotIncludeDir)
	end
end

function generate.generateWorkspaceProjects(wks)
	if type(wks.sourcedir) ~= "string" then
		return
	end

	workspace(wks.name)

	-- Get the absolute path to the workspace source directory
	local sourcedir = path.isabsolute(wks.sourcedir) and wks.sourcedir or path.join(wks.basedir, wks.sourcedir)

	-- Recurse through the workspace directory and include all premake scripts
	local premake_script_pattern = path.join(sourcedir, "**/premake5.lua")
	local script_paths = os.matchfiles(premake_script_pattern)

	-- Invoke Project Scripts
	for _, script_path in ipairs(script_paths) do
		local script_dir = path.getdirectory(script_path)
		local script_file = path.getname(script_path)

		local cwd = os.getcwd()
		os.chdir(script_dir)

		local nProjects = #wks.projects

		local script_fn = loadfile(script_file)
		assert(script_fn)()

		for index = nProjects + 1, #wks.projects do
			local prj = wks.projects[index]
			project(prj.name); do
				-- Override Point
				p.callArray(oyl3d.elements.workspaceProject, prj)
				includedirs {
					generate.prjDotIncludeDir(prj)
				}
			end
		end

		os.chdir(cwd)
	end
end

function generate.generatePackageProjects(wks)
	if type(wks.packageprojects) ~= "table" then
		return
	end

	workspace(wks.name)

	local package_group = wks.packageprojects.group
	local packages = wks.packageprojects.packages

	for name, package in spairs(packages) do
		local package_fetch_info = package_fetch_table[name]
		local package_local_path = os.getcwd()
		if type(package_fetch_info) == "table" then
			package_local_path = package_fetch_info.Local and package_fetch_info.Local.Path or nil
			if not package_local_path then
				package_local_path = path.join(_MAIN_SCRIPT_DIR, "Packages", name)
			end
		end

		local cwd = os.getcwd()
		os.chdir(package_local_path)

		group(package_group)
		project(name); do
			local prj = p.api.scope.project

			prj._package = package

			location(path.join(wks.basedir, "Packages", prj.name))
			warnings "Off"

			if type(package.OnProject) == "function" then
				package.OnProject(prj)
				project(name)
			end

			-- use cwd since it's a cache of the parent cwd
			if prj.basedir:lower() == cwd:lower() then
				error(
					string.format("Cannot infer basedir for package \"%s\"! Did you forget to call basedir()?", name)
				)
			end
			private.errorIfPackageNotOnDisk(prj)

			-- If package has a premake script in the basedir, run it
			local script_dir = prj.basedir
			local script_file = "premake5.lua"
			local cwd = os.getcwd()
			os.chdir(script_dir)
			
			if os.isfile(path.join(script_dir, script_file)) then
				local script_fn = loadfile(script_file)
				assert(script_fn)()
				
				-- Included project from script must match name of package
				if p.api.scope.project ~= prj then
					error(
						string.format(
							"Name of project \"%s\" from project script \"%s\" does not including package \"%s\"",
							p.api.scope.project.name,
							p.api.scope.project.script,
							name
						)
					)
				end
			end
			
			-- Override Point
			p.callArray(oyl3d.elements.packageProject, prj)
			
			os.chdir(cwd)
		end

		os.chdir(cwd)
	end
end

function private.errorIfPackageNotOnDisk(prj)
	local ok = os.isdir(prj.basedir)
	if not ok then
		term.pushColor(term.errorColor); do
			io.write(('Directory "%s" for Package "%s" not found! Did you run '):format(prj.basedir, prj.name))

			term.pushColor(term.infoColor); do
				io.write("premake packages")
			end
			term.popColor()

			io.write("?\n")
		end
		term.popColor()

		error(err)
	end
end

function generate.removeNonProjectPackages(wks)
	if not p.action.isConfigurable() then
		return
	end
	
	-- Remove all references to non-project packages
	-- Do this before we remove the projects from the workspace so we can distiguish between a raw
	-- library links and package links
	for _, prj in ipairs(wks.projects) do
		for linkindex = #prj.links, 1, -1 do
			local link = prj.links[linkindex]
			local linkprj = wks.projects[link]
			if linkprj and linkprj._package and not linkprj._package.OnProject then
				project(prj.name)
				removelinks {
					link
				}
			end
		end
	end
	workspace()
	
	-- Remove all non-project packages from the workspace
	-- We only want the link as an indicator to call the OnDepend function. 
	-- If no project is generated, there is no library to depend on
	for prjindex = #wks.projects, 1, -1 do
		local prj =  wks.projects[prjindex]
		if prj._package and not prj._package.OnProject then
			table.remove(wks.projects, prjindex)
		end
	end
end

function generate.removeUnreferencedProjects(wks)
	if not p.action.isConfigurable() then
		return
	end
	
	-- Gather the set of projects referencing or being referenced by another project
	local dependSet = {}
	for _, prj in pairs(wks.projects) do
		if #prj.links > 0 then
			dependSet[prj.name] = true
		end
		for _, link in ipairs(prj.links) do
			dependSet[link] = true
		end
		for _, depend in ipairs(prj.dependson) do
			dependSet[depend] = true
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
end

return m
