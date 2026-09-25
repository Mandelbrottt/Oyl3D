local Config = require "Config"

local p = premake

local oyl3d = p.modules.oyl3d
oyl3d.project = oyl3d.project or {}
oyl3d.generate = oyl3d.generate or {}

local m = oyl3d.project
local generate = oyl3d.generate

local private = {}

local cpp_file_patterns = {
	"*.cpp",
	"*.h",
	"*.inl",
	"*.natvis",
	"*.hlsl",
}

--- Add the Project File List to the active project
---@see Project.GetFileList
function m.files()
	filter { "language:c++" }; do
		local fileList = m.getFileList(cpp_file_patterns)
		files(fileList)
	end
	filter {}
end

--- Get the list of files to be added to the current project
--- File list is the set of all files in recursive directories without another premake5.lua script
---@return string[] The list of files to be associated with the project in the current working directory
function m.getFileList(file_patterns_table)
	assert(type(file_patterns_table) == "table")

	-- Generate list of project directories, defined as any subdirectory without
	local premakeDirs = table.translate(
		os.matchfiles("*/**premake5.lua"),
		function(value) return path.getdirectory(value) end
	)

	-- Iterate premake dirs, if projectDir is a child (recursive) of premakeDir, remove it
	local projectDirs = os.matchdirs("**")
	local index = 1
	while index <= #projectDirs do
		local dir = projectDirs[index]
		for _, premakeDir in ipairs(premakeDirs) do
			-- Check if projectDir is a child of premakeDir. If so, remove it
			if string.contains(dir, premakeDir) then
				table.remove(projectDirs, index)

				-- Subtract 1 from index after removal, so that
				-- for loop will stay at the same index next loop
				index = index - 1
				break
			end
		end
		index = index + 1
	end

	-- Insert empty string to represent the root directory
	table.insert(projectDirs, "")

	-- Iterate all project directories, and add patterns for common cpp files
	local result = {}
	for _, dir in ipairs(projectDirs) do
		local patterns = table.translate(
			file_patterns_table,
			function(pattern) return path.join(dir, pattern) end
		)
		for _, pattern in ipairs(patterns) do
			table.insert(result, pattern)
		end
	end

	return result
end

function generate.prjDotIncludeDir(prj)
	local wks = prj.workspace
	return path.join(generate.wksDotIncludeDir(wks), prj.name)
end

function generate.createPrjDotIncludeDirectory(prj)
	local wks = prj.workspace

	generate.createWksDotIncludeDir(wks)

	local prjDotIncludeDir = generate.prjDotIncludeDir(prj)
	if os.isdir(prjDotIncludeDir) then
		return
	end

	os.mkdir(prjDotIncludeDir)
end

function generate.prjAddEntryToDotInclude(prj, source_path, entry_name)
	generate.createPrjDotIncludeDirectory(prj)

	if not entry_name then
		entry_name = path.getname(source_path)
	end

	local prj_dot_include = generate.prjDotIncludeDir(prj)
	local link_path = path.join(prj_dot_include, entry_name)
	os.linkdir(source_path, link_path)
end

function generate.applyProjectDefaults(prj)
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
	-- Force a new block since we did some shenanigans
	project(prj.name)
end

function generate.applySharedToStaticLib(prj)
	if prj.lockkind then
		return
	end

	project(prj.name)
	filter { "platforms:Standalone", "kind:SharedLib" }; do
		kind "StaticLib"
	end; filter {}
end

function generate.generateReflectionInfo(prj)
	local project_action_call_array = oyl3d.elements.workspaceAction()
	assert(type(project_action_call_array) == "table")

	if not prj.reflection then
		-- Check if we've already overridden projectAction. If not, then override it
		if not table.contains(project_action_call_array, private.deleteDotGeneratedFolder) then
			premake.override(oyl3d.elements, "projectAction", function(base, prj)
				local result = base(prj)
				table.insert(result, private.deleteDotGeneratedFolder)
				return result
			end)
		end
		return
	end

	project(prj.name)

	dependson {
		"Oyl.Spyll",
	}

	-- Handle this logic in tokenized strings to ensure proper project filters are applied
	local spyllCommand = path.join(Config.BinariesDir, "Oyl.Spyll.exe")

	local function appendToCommand(str)
		-- Append trimmed string after space
		spyllCommand = spyllCommand .. " " .. str:match("^%s*(.-)%s*$")
	end

	-- Oyl.Spyll Commands
	appendToCommand [[%{table.concat(
		table.translate(
				table.filter(
					cfg.files,
					function(file) return path.hasextension(file, ".h") end
				),
				function(file) return '"' .. file .. '"' end
			),
			" "
		)}]]

	appendToCommand '"-assembly=%{cfg.buildtarget.basename}"'
	appendToCommand [[
		%{table.concat(
			table.translate(
				cfg.links,
				function(link) return '"-dependency=' .. link .. '"' end
			), 
			" "
		)}
	]]

	-- CC1 commands
	appendToCommand '--'
	appendToCommand '-xc++' -- language
	appendToCommand '"-std=%{cfg.cppdialect:lower()}"' -- C++ Standard

	-- Include directories
	appendToCommand [[
		%{table.concat(
			table.translate(
				cfg.includedirs,
				function(dir) return '"-I' .. dir .. '"' end
			), 
			" "
		)}
	]]
	
	-- External Include Directories
	appendToCommand [[
		%{table.concat(
			table.translate(
				cfg.externalincludedirs,
				function(dir) return '"-isystem' .. dir .. '"' end
			), 
			" "
		)}
	]]
	if p.action.current() and p.action.current().vstudio then
		appendToCommand '"-isystem$(IncludePath)"'
	end

	-- Defines
	appendToCommand [[
		%{table.concat(
			table.translate(
				cfg.defines,
				function(define) return '-D' .. define end
			), 
			" "
		)}
	]]

	-- Only include --pch arg if project has a pch
	-- Force include the pch file
	appendToCommand [[%{cfg.pchheader and '"-include' .. cfg.pchheader .. '"' or ''}]]

	prebuildmessage("Executing " .. spyllCommand)

	prebuildcommands {
		"cd %{prj.location}",
		spyllCommand
	}

	files {
		path.join("%{wks.basedir}", "GeneratedInclude.cpp")
	}

	-- Check if we've already overridden projectAction. If not, then override it
	if not table.contains(project_action_call_array, private.addDotGeneratedToPrjDotIncludeDir) then
		premake.override(oyl3d.elements, "projectAction", function(base, prj)
			local result = base(prj)
			table.insert(result, private.addDotGeneratedToPrjDotIncludeDir)
			return result
		end)
	end

	-- Ensure files under .Generated aren't included in the build
	local generatedDir = ".Generated"
	local generatedFilesPattern = path.join(generatedDir, "**")
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

function private.addDotGeneratedToPrjDotIncludeDir(prj)
	-- Run on projects that have reflection enabled
	if not prj.reflection then
		return
	end
	
	local source_dir = path.join(prj.basedir, ".Generated")
	generate.prjAddEntryToDotInclude(prj, source_dir)
end

function private.deleteDotGeneratedFolder(prj)
	-- Run on projects that have reflection disabled
	if prj.reflection then
		return
	end
	
	local dot_generated_folder = path.join(prj.basedir, ".Generated")
	if os.isdir(dot_generated_folder) then
		os.rmdir(dot_generated_folder)
	end
end

function generate.connectProjectLinks(prj)
	local wks = prj.workspace

	project(prj.name)

	for linkindex = #prj.links, 1, -1 do
		local link = prj.links[linkindex]
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

		local package = linkprj._package
		if package then
			if type(package.OnDepend) == "function" then
				local prj_copy = private.bakeConfigsForPrj(prj)
				local linkprj_copy = private.bakeConfigsForPrj(linkprj)

				-- Iterate over each config in prj, find the matching config in linkprj, then call package.OnDepend
				-- with a filter on the config and platform
				for cfg in p.project.eachconfig(prj_copy) do
					local linkcfg = p.project.getconfig(linkprj_copy, cfg.buildcfg, cfg.platform)
					if not linkcfg then
						linkcfg = p.project.findClosestMatch(linkprj_copy, cfg.buildcfg, cfg.platform)
					end
					filter { "configurations:" .. cfg.buildcfg, "platforms:" .. cfg.platform }; do
						local nBlocks = #prj.blocks
						local cwd = os.getcwd()
						os.chdir(linkcfg.basedir)
						package.OnDepend(cfg, linkcfg)
						os.chdir(cwd)
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

		::continue::
	end
end

function generate.createProjectLinkDirs(prj)
	local wks = prj.workspace

	generate.prjAddEntryToDotInclude(prj, prj.basedir)

	for _, link in ipairs(prj.links) do
		local linkprj = wks.projects[link]
		if not linkprj then
			goto continue
		end
		-- Non-source project, disregard
		if linkprj.basedir == wks.basedir then
			goto continue
		end

		local link_dirs = linkprj.packageincludedirs

		if not link_dirs then
			-- if unset, use basedir
			link_dirs = linkprj.basedir
		end
		
		-- Add symlink from link dir to project .Include folder
		if type(link_dirs) ~= "table" then
			local link_dir = link_dirs
			local link_name = path.getname(link_dir)
			if linkprj._package and link_dir == linkprj.basedir then
				link_name = linkprj.name
			end
			generate.prjAddEntryToDotInclude(prj, link_dir, link_name)
		else
			for _, link_dir in ipairs(link_dirs) do
				generate.prjAddEntryToDotInclude(prj, link_dir)
			end
		end


		::continue::
	end
end

-- Abridged from premake source - "self" parameter name kept for ease of use
function private.bakeConfigsForPrj(prj)
	if prj._baked_config_copy then
		return prj._baked_config_copy
	end

	local wks = prj.workspace

	local function bakeCopyOfPremakeSource(self)
		local oven = p.oven
		local context = p.context
		self.terms = self.terms or {}
		wks.terms = wks.terms or {}

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

	local prj_copy = table.deepcopy(prj)
	bakeCopyOfPremakeSource(prj_copy)
	prj._baked_config_copy = prj_copy

	return prj_copy
end

return m
