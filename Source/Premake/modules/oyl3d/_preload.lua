local p = premake

premake.modules.oyl3d = p.modules.oyl3d or {}
premake.modules.oyl3d._VERSION = "0.0.1"
premake.oyl3d = p.modules.oyl3d

include("actions.lua")
include("clean_action.lua")
include("packages_action.lua")
include("vstudio_action.lua")

p.api.register {
	name = "sourcedir",
	scope = "workspace",
	kind = "path"
}

---	{
---		["C++"]: fun(),
---     ...
---	}
p.api.register {
	name = "projectdefaults",
	scope = "workspace",
	kind = "table",
	allowed = function(tbl)
		local allowed = true
		local function check(condition) allowed = allowed and condition end

		check(#tbl == 0)
		for language, func in pairs(tbl) do
			check(type(language) == "string")
			check(type(func) == "function")
			check(table.contains(premake.field._loweredList["language"].allowed, language:lower()))
		end
		
		return allowed;
	end,
}

---	{
---		group: string
---		packages: { 
---			[string]: { 
---				OnProject: fun(prj), 
---				OnDepend: fun(packageprj) 
---			} 
---		}
---	}
p.api.register {
	name = "packageprojects",
	scope = "workspace",
	kind = "table",
	allowed = function(tbl)
		local allowed = true
		local function check(condition) allowed = allowed and condition end
		
		check(tbl.group == nil or type(tbl.group) == "string")

		check(type(tbl.packages) == "table")
		check(#tbl.packages == 0)
		for name, package in pairs(tbl.packages) do
			check(type(name) == "string")
			check(type(package) == "table")
			check(type(package.OnProject) == "function")
			check(package.OnDepend == nil or type(package.OnDepend) == "function")
		end
		
		return allowed;
	end,
}

p.api.register {
	name = "checkproject",
	scope = "workspace",
	kind = "boolean",
}

newoption {
	trigger     = "no-premake-check",
	description = "Disable the automatic run of premake on every compile",
}

newoption {
	trigger = "premake-check",
	description = "Return a non-zero exit code if any files on disk are modified by the current action",
}

p.api.register {
	name = "packageincludedir",
	scope = "project",
	kind = "path",
}

p.api.register {
	name = "lockkind",
	scope = "project",
	kind = "boolean"
}

p.api.register {
	name = "reflection",
	scope = "project",
	kind = "boolean"
}

p.api.register {
	name = "reflectionshowgenerated",
	scope = "project",
	kind = "boolean"
}

-- Always load
return function(cfg) return true end