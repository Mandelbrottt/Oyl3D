local p = premake

p.oyl3d = p.modules.oyl3d

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

p.api.register {
	name = "checkproject",
	scope = "workspace",
	kind = "boolean",
}

newoption {
	trigger     = "no-premake-check",
	description = "Disable the automatic run of premake on every compile",
}

-- Always load
return function(cfg) return true end