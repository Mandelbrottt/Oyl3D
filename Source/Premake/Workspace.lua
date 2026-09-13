local Project = require "Project"

local Workspace = {}

---@return any
function Workspace.Current()
	local function recurseParent(scope)
		if scope.class.name == "workspace" or scope.class.alias == "workspace" then
			return scope
		elseif scope.parent then
			return recurseParent(scope.parent)
		end
		return nil
	end
	local scope = premake.api.scope.current
	return recurseParent(scope)
end

function Workspace.GenerateProjects()
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

function Workspace.GetWorkspaceProjects(wks)
	local result = {}
	for _, prj in ipairs(wks.projects) do
		if prj._package then
			goto continue
		end
		if prj.basedir == wks.basedir then
			goto continue
		end
		
		local script = path.normalize(path.getdirectory(prj.script))
		local location = wks.location
		if location then
			if not path.isabsolute(location) then
				location = path.normalize(path.join(wks.basedir, location))
			end
		else
			location = wks.basedir
		end
		if string.contains(script, location) then
			table.insert(result, prj)
		end

		::continue::
	end
	return result
end

return Workspace
