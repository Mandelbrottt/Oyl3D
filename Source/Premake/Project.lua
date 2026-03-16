local Project = {}

---@return any
function Project.CurrentProject()
	-- Recursively iterate up the scope tree until the project scope is found, else return nil
	local scope = premake.api.scope.current
	while true do
		if not scope.parent then
			return nil
		end

		if scope.class.name == "project" then
			return scope
		end
		
		scope = scope.parent
	end
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
