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

function oyl3d.prjDotIncludeDir(prj)
	local wks = prj.workspace
	return path.join(oyl3d.wksDotIncludeDir(wks), prj.name)
end

function oyl3d.createPrjDotIncludeDirectory(prj)
	local wks = prj.workspace

	oyl3d.createWksDotIncludeDir(wks)

	local prjDotIncludeDir = oyl3d.prjDotIncludeDir(prj)
	if os.isdir(prjDotIncludeDir) then
		return
	end

	os.mkdir(prjDotIncludeDir)
end

function oyl3d.prjAddEntryToDotInclude(prj, source_path, entry_name)
	oyl3d.createPrjDotIncludeDirectory(prj)

	if not entry_name then
		entry_name = path.getname(source_path)
	end

	local prj_dot_include = oyl3d.prjDotIncludeDir(prj)
	local link_path = path.join(prj_dot_include, entry_name)
	os.linkdir(source_path, link_path)
end

function oyl3d.addDotGeneratedToPrjDotIncludeDir(prj)
	-- Run on projects that have reflection enabled
	if not prj.reflection then
		return
	end

	local source_dir = path.join(prj.basedir, ".Generated")
	oyl3d.prjAddEntryToDotInclude(prj, source_dir)
end

function oyl3d.deleteDotGeneratedFolder(prj)
	-- Run on projects that have reflection disabled
	if prj.reflection then
		return
	end

	local dot_generated_folder = path.join(prj.basedir, ".Generated")
	if os.isdir(dot_generated_folder) then
		os.rmdir(dot_generated_folder)
	end
end

-- -- Abridged from premake source - "self" parameter name kept for ease of use
-- function private.bakeConfigsForPrj(prj)
-- 	if prj._baked_config_copy then
-- 		return prj._baked_config_copy
-- 	end

-- 	print("PRE_BAKING ", prj.name)

-- 	local wks = prj.workspace

-- 	local function bakeCopyOfPremakeSource(self)
-- 		local oven = p.oven
-- 		local context = p.context
-- 		self.terms = self.terms or {}
-- 		wks.terms = wks.terms or {}

-- 		-- vvv FROM PREMAKE SOURCE oven.lua:260 vvv
-- 		self.location = self.location or self.basedir
-- 		context.basedir(self, self.location)

-- 		local cfgs = table.fold(self.configurations or {}, self.platforms or {})
-- 		oven.bubbleFields(self, self, cfgs)
-- 		self._cfglist = oven.bakeConfigList(self, cfgs)

-- 		-- Don't allow a project-level system setting to influence the configurations

-- 		self.system = nil

-- 		-- Finally, step through the list of configurations I built above and
-- 		-- bake all of those down into configuration contexts as well. Store
-- 		-- the results with the project.

-- 		self.configs = {}

-- 		for _, pairing in ipairs(self._cfglist) do
-- 			local buildcfg = pairing[1]
-- 			local platform = pairing[2]
-- 			local cfg = oven.bakeConfig(wks, self, buildcfg, platform)

-- 			if p.action.supportsconfig(p.action.current(), cfg) then
-- 				self.configs[(buildcfg or "*") .. (platform or "")] = cfg
-- 			end
-- 		end
-- 		-- ^^^ FROM PREMAKE SOURCE ^^^
-- 	end

-- 	local prj_copy = table.deepcopy(prj)
-- 	bakeCopyOfPremakeSource(prj_copy)
-- 	prj._baked_config_copy = prj_copy

-- 	return prj_copy
-- end

return m
