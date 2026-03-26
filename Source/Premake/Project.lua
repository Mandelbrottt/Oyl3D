local Project = {}

local CppFilePatterns = {
	"*.cpp",
	"*.h",
	"*.inl",
	"*.natvis",
}

--- Run the script at script_path
---@param script_path string Path to the script to execute
function Project.Script(script_path)
	if path.getname(script_path) ~= "premake5.lua" and not path.hasextension(script_path, ".lua") then
		script_path = path.join(script_path, "premake5.lua")
	end
	assert(loadfile(script_path))()
end

---@return any
function Project.CurrentProject()
	local scope = premake.api.scope.current
	if scope.class.name == "project" then
		return scope
	else
		return nil
	end
end

---@param callback fun()
function Project.NestedFilter(callback)
	-- Cache the current scope, block, and block index
	local scope = assert(Project.CurrentProject())
	local currentBlock = scope.current
	local blocks = scope.blocks
	local lastBlockIndex = #blocks

	-- Run the callback
	callback()

	-- Add existing filters to nested calls to filter
	if #currentBlock._criteria.terms ~= 0 then
		for i = lastBlockIndex + 1, #blocks, 1 do
			local block = blocks[i]
			local newTerms = table.join(block._criteria.terms, currentBlock._criteria.terms)
			local crit = premake.criteria.new(newTerms)
			block._criteria = crit
		end
	end

	-- Restore outer filter
	scope.current = currentBlock
end

---@return string
function Project.InsideProjectMacro()
	local prj = Project.CurrentProject()
	local projectName = prj.name:gsub("[%.%-]", "_")
	return string.format("_INSIDE_%s=1", projectName:upper())
end

---@return string
function Project.CurrentAssemblyMacro()
	local prj = Project.CurrentProject()
	local projectName = prj.name:gsub("[%.%-]", "_")
	return string.format("_CURRENT_ASSEMBLY=\"%s\"", projectName)
end

--- Get the list of files to be added to the current project
--- File list is the set of all files in recursive directories without another premake5.lua script
---@return string[] The list of files to be associated with the project in the current working directory
function Project.GetFileList()
	-- Generate list of project directories, defined as any subdirectory without
	local premakeDirs = table.translate(
		os.matchfiles("*/**premake5.lua"),
		function(value) return path.getdirectory(value) end
	)

	-- Iterate premake dirs, if projectDir is a child (recursive) of premakeDir, remove it
	local projectDirs = os.matchdirs("**")
	for index = 1, #projectDirs do
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
	end

	-- Insert empty string to represent the root directory
	table.insert(projectDirs, "")

	-- Iterate all project directories, and add patterns for common cpp files
	local result = {}
	for _, dir in ipairs(projectDirs) do
		local patterns = table.translate(
			CppFilePatterns,
			function(pattern) return path.join(dir, pattern) end
		)
		for _, pattern in ipairs(patterns) do
			table.insert(result, pattern)
		end
	end

	return result
end

--- Add the Project File List to the active project
---@see Project.GetFileList
function Project.Files()
	local fileList = Project.GetFileList()
	files(fileList)
end

return Project
