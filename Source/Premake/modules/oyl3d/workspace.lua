local Config = require "Config"
local Packages = require "Packages"

local p = premake

local oyl3d = p.modules.oyl3d
oyl3d.workspace = oyl3d.workspace or {}

local m = oyl3d.workspace

function m.wksDotIncludeDir(wks)
	return path.join(wks.basedir, ".Include")
end

function m.createWksDotIncludeDir(wks)
	local dotIncludeDir = m.wksDotIncludeDir(wks)

	if os.isdir(dotIncludeDir) then
		return
	end

	-- Make .Include dir and mark the directory as hidden
	os.mkdir(dotIncludeDir)
	if os.host() == premake.WINDOWS then
		os.executef("attrib +h %s /s /d", dotIncludeDir)
	end
end

function m.prepareWorkspace(wks)
	local wksDotIncludeDir = path.join(wks.basedir, ".Include")

	if os.isdir(wksDotIncludeDir) then
		os.rmdir(wksDotIncludeDir)
	end
end

function m.generateWorkspaceProjects(wks)
	if type(wks.sourcedir) ~= "string" then
		return
	end

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
				oyl3d.project.createPrjDotIncludeDirectory(prj)
				oyl3d.project.generateReflectionInfo(prj)
			end
		end

		os.chdir(cwd)
	end
end

function m.removeUnreferencedProjects(wks)
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
end

return m
