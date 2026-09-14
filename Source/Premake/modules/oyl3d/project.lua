local Config = require "Config"

local p = premake

local oyl3d = p.modules.oyl3d
oyl3d.project = oyl3d.project or {}
oyl3d.generate = oyl3d.generate or {}

local m = oyl3d.project
local generate = oyl3d.generate

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

	project(prj.name)
end

function generate.applySharedToStaticLib(prj)
	if prj.lockkind then
		return
	end

	filter { "platforms:Standalone", "kind:SharedLib" }; do
		kind "StaticLib"
	end; filter {}
end

function generate.generateReflectionInfo(prj)
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
	local linkDir = path.join(generate.prjDotIncludeDir(prj), generatedDir)
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

function generate.connectProjectLinks(prj)
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
				local prj_copy = generate.bakeConfigsForPrj(prj)
				local linkprj_copy = generate.bakeConfigsForPrj(linkprj)

				-- Iterate over each config in prj, find the matching config in linkprj, then call package.OnDepend
				-- with a filter on the config and platform
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

		-- Create a symlink of the link project folder
		local prjDotIncludeLinkDir = path.join(generate.prjDotIncludeDir(prj), link_name)
		os.linkdir(link_dir, prjDotIncludeLinkDir)

		::continue::
	end
end

-- Abridged from premake source - "self" parameter name kept for ease of use
function generate.bakeConfigsForPrj(prj)
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
