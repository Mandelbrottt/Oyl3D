local Project = {}

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

return Project
