local Check = require "CheckProject"
local Config = require "Config"
local Packages = require "Packages"

local p = premake

p.modules.oyl3d = p.modules.oyl3d or {}
p.modules.oyl3d._VERSION = "0.0.1"

local m = p.modules.oyl3d

include("_preload.lua")

p.override(p.main, "preBake", function(base)
	base()
	m.preBake()
end)

-- p.override(p.main, "postBake", function(base)
-- 	base()
-- 	m.postBake()
-- end)

function m.preBake()
	local global = p.api.scope.global
	for _, wks in ipairs(global.workspaces) do
		local cwd = os.getcwd()
		os.chdir(wks.basedir)

		workspace(wks.name); do
			m.prepareWorkspace(wks)

			m.generateWorkspaceProjects(wks)
			m.generatePackageProjects(wks)

			for _, prj in ipairs(wks.projects) do
				local cwd = os.getcwd()
				os.chdir(prj.basedir)

				project(prj.name); do
					m.applyProjectDefaults(prj)
					m.applySharedToStaticLib(prj)

					m.connectProjectLinks(prj)
				end
				os.chdir(cwd)
			end

			m.removeUnreferencedProjects(wks)

			m.generateCheckProject(wks)
		end
		os.chdir(cwd)
	end
end

function m.postBake()
	local global = p.api.scope.global
	for _, wks in ipairs(global.workspaces) do
		local cwd = os.getcwd()
		os.chdir(wks.basedir)

		workspace(wks.name); do
			for _, prj in ipairs(wks.projects) do
				local cwd = os.getcwd()
				os.chdir(prj.basedir)

				project(prj.name); do
					m.projectLinksOnDepend(prj)
				end
				os.chdir(cwd)
			end
		end
		os.chdir(cwd)
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
				m.createPrjDotIncludeDirectory(prj)
				m.generateReflectionInfo(prj)
			end
		end

		os.chdir(cwd)
	end
end

function m.generateReflectionInfo(prj)
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

	if p.action.current() and p.action.current().vstudio then
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
	local linkDir = path.join(m.prjDotIncludeDir(prj), generatedDir)
	os.linkdir(generatedDir, linkDir)

	local generatedFilesPattern = path.join("%{prj.location}", generatedDir, "**")

	if prj.reflectionshowgenerated then
		filter { "files:" .. generatedFilesPattern }; do
			excludefrombuild "On"
		end; filter {}
	else
		removefiles {
			generatedFilesPattern
		}
	end
end

function m.generatePackageProjects(wks)
	if type(wks.packageprojects) ~= "table" then
		return
	end

	local package_group = wks.packageprojects.group
	local packages = wks.packageprojects.packages

	for name, package in spairs(packages) do
		local package_fetch_info = Packages[name] -- Grab from "Packages" intentionally
		local package_local_path = os.getcwd()
		if type(package_fetch_info) == "table" then
			package_local_path = package_fetch_info.Local and package_fetch_info.Local.Path or nil
			if not package_local_path then
				package_local_path = path.join(Config.PackageCacheDir, name)
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

			package.OnProject(prj)
			project(name)
			
			-- use cwd since it's a cache of the parent cwd
			if prj.basedir:lower() == cwd:lower() then
				error(string.format("Cannot infer basedir for package \"%s\"! Did you forget to call basedir()?", name))
			end

			m.errorIfPackageNotOnDisk(prj)

			-- If package has a premake script in the basedir, run it
			local script_dir = prj.basedir
			local script_file = "premake5.lua"
			if os.isfile(path.join(script_dir, script_file)) then
				local cwd = os.getcwd()
				os.chdir(script_dir)

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

				os.chdir(cwd)
			end
		end

		os.chdir(cwd)
	end
end

function m.errorIfPackageNotOnDisk(prj)
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

function m.applyProjectDefaults(prj)
	local wks = prj.workspace
	local defaults = wks.projectdefaults
	if not defaults then
		return
	end

	local nBlocks = #prj.blocks

	local default_func
	for language, func in pairs(defaults) do
		if prj.language:lower() == language:lower() then
			default_func = func
		end
	end

	if not default_func then
		return
	end

	-- Force a new block
	project(prj.name)
	default_func()

	-- Move all newly created blocks to the beginning of the blocks list
	if nBlocks < #prj.blocks then
		-- For some reason in-place table.move isn't working, so use a middleman table
		local new_blocks = table.move(prj.blocks, nBlocks + 1, #prj.blocks, 1, {})
		table.move(prj.blocks, 1, nBlocks, #new_blocks + 1, new_blocks)
		prj.blocks = new_blocks
	end

	project(prj.name)
end

function m.applySharedToStaticLib(prj)
	if prj.lockkind then
		return
	end

	filter { "platforms:Standalone", "kind:SharedLib" }; do
		kind "StaticLib"
	end; filter {}
end

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

function m.prjDotIncludeDir(prj)
	local wks = prj.workspace
	return path.join(m.wksDotIncludeDir(wks), prj.name)
end

function m.createPrjDotIncludeDirectory(prj)
	local wks = prj.workspace

	m.createWksDotIncludeDir(wks)

	local prjDotIncludeDir = m.prjDotIncludeDir(prj)
	if os.isdir(prjDotIncludeDir) then
		os.rmdir(prjDotIncludeDir)
	end
	os.mkdir(prjDotIncludeDir)

	includedirs { prjDotIncludeDir }

	-- Add link to own basedir in .Include dir
	os.linkdir(
		prj.basedir,
		path.join(prjDotIncludeDir, path.getname(prj.basedir))
	)
end

function m.connectProjectLinks(prj)
	local wks = prj.workspace

	for _, link in ipairs(prj.links) do
		local linkprj = wks.projects[link]
		if not linkprj then
			goto continue
		end

		-- recurse links, add all children as links to parent projects for non-editor platform
		-- local function addLinksRecursive(link)
		-- 	links { link.links }
		-- 	libdirs { link.libdirs }
		-- 	for _, link in ipairs(link.links) do
		-- 		local prj = wks.projects[link]
		-- 		if prj then
		-- 			addLinksRecursive(prj)
		-- 		end
		-- 	end
		-- end
		-- filter "platforms:not *Editor*"; do
		-- 	addLinksRecursive(linkprj)
		-- end
		-- filter {}

		-- Add symlink from link dir to .Include/prj.name/link.name
		local link_dir = linkprj.packageincludedir or linkprj.basedir
		local link_name = path.getname(link_dir)

		local package = linkprj._package
		if package then
			-- use the package name as the link directory name
			link_name = linkprj.name
			if type(package.OnDepend) == "function" then
				local function bakeConfigsForPrj(self)
					local oven = p.oven
					local context = p.context
					self.terms = {}
					if not wks.terms then
						wks.terms = {}
					end
					
					-- vvv FROM PREMAKE SOURCE oven.lua:260 vvv
					self.location = self.location or self.basedir
					context.basedir(self, self.location)

					local cfgs = table.fold(self.configurations or {}, self.platforms or {})
					oven.bubbleFields(self, self, cfgs)
					self._cfglist = oven.bakeConfigList(self, cfgs)

					-- Don't allow a project-level system setting to influence the configurations

					self.system = nil

					-- Finally, step through the list of configurations I built above and
					-- bake all of those down into configuration contexts as well. Store
					-- the results with the project.

					self.configs = {}

					for _, pairing in ipairs(self._cfglist) do
						local buildcfg = pairing[1]
						local platform = pairing[2]
						local cfg = oven.bakeConfig(wks, self, buildcfg, platform)

						if p.action.supportsconfig(p.action.current(), cfg) then
							self.configs[(buildcfg or "*") .. (platform or "")] = cfg
						end
					end
					-- ^^^ FROM PREMAKE SOURCE ^^^
				end
				
				local prj_copy = prj._baked_config_copy
				if not prj_copy then
					prj_copy = table.deepcopy(prj)
					bakeConfigsForPrj(prj_copy)
					prj._baked_config_copy = prj_copy
				end
				
				local linkprj_copy = linkprj._baked_config_copy
				if not linkprj_copy then
					linkprj_copy = table.deepcopy(linkprj)
					bakeConfigsForPrj(linkprj_copy)
					linkprj._baked_config_copy = linkprj_copy
				end

				for cfg in p.project.eachconfig(prj_copy) do
					local linkcfg = p.project.getconfig(linkprj_copy, cfg.buildcfg, cfg.platform)
					if not linkcfg then
						linkcfg = p.project.findClosestMatch(linkprj_copy, cfg.buildcfg, cfg.platform)
					end
					filter { "configurations:" .. cfg.buildcfg, "platforms:" .. cfg.platform }; do
						local nBlocks = #prj.blocks
						package.OnDepend(cfg, linkcfg)
						-- Apply config and platforms criteria to all filters added in OnDepend
						for i = nBlocks + 1, #prj.blocks do
							local block = prj.blocks[i]
							local new_terms = table.join(block._criteria.terms, prj.blocks[nBlocks]._criteria.terms)
							local criteria = p.criteria.new(new_terms)
							block._criteria = criteria
						end
					end
				end
			end
		end

		local prjDotIncludeLinkDir = path.join(m.prjDotIncludeDir(prj), link_name)
		os.linkdir(link_dir, prjDotIncludeLinkDir)

		::continue::
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

function m.generateCheckProject(wks)
	if wks.checkproject then
		Check.GenerateProject()
	end
end

function m.projectLinksOnDepend(prj)
	local wks = prj.workspace

	for _, link in ipairs(prj.links) do
		local linkprj = wks.projects[link]
		local package = linkprj._package
		if not (package and type(package.OnDepend) == "function") then
			goto continue
		end

		for cfg in p.project.eachconfig(prj) do
			local linkcfg = p.project.getconfig(linkprj, cfg.buildcfg, cfg.platform)
			if not linkcfg then
				linkcfg = p.project.getClosestMatch(linkprj, cfg.buildcfg, cfg.platform)
			end
			filter { "configurations:" .. cfg.buildcfg, "platforms:" .. cfg.platform }; do
				local nBlocks = #prj.blocks
				package.OnDepend(cfg, linkcfg)
				-- Ensure any filters added in OnDepend also apply to the right configurations
				for i = nBlocks + 1, #prj.blocks do
					local block = prj.blocks[i]
					local new_terms = table.join(block._criteria.terms, prj.blocks[nBlocks]._criteria.terms)
					local criteria = p.criteria.new(new_terms)
					block._criteria = criteria
				end
			end
		end
		::continue::
	end
end

return m
