local Config = require "Config"

local p = premake

local oyl3d = p.modules.oyl3d
oyl3d.check = oyl3d.check or {}

local m = oyl3d.check

function m.generateCheckProject(wks)
	if _OPTIONS["no-premake-check"] then
		return
	end
	
	if not wks.checkproject then
		return
	end

	m.addCheckLinkToProjects(wks)
	m.checkProjectDefinition()
end

function m.addCheckLinkToProjects(wks)
	local cwd = os.getcwd()
	for _, prj in ipairs(wks.projects) do
		os.chdir(prj.basedir)

		project(prj.name); do
			links { "Premake" }
		end
	end
	os.chdir(cwd)
end

function m.checkProjectDefinition()
	group "Premake"

	project "Premake"; do
		kind "Makefile"
		filename("%{prj.name}")
		targetdir(Config.BinariesDir)
		objdir(Config.ObjectDir)

		local premakeCommandArgs = {
			_ACTION,
			"--file=" .. _MAIN_SCRIPT,
			"--premake-check",
			"--cc=%{cfg.toolset}",
		}

		local premakeCommand = "premake5"
		for _, arg in ipairs(premakeCommandArgs) do
			premakeCommand = string.format("%s %s", premakeCommand, arg)
		end

		if additionalArgs then
			for _, arg in ipairs(additionalArgs) do
				premakeCommand = string.format("%s %s", premakeCommand, arg)
			end
		end

		buildcommands {
			premakeCommand,
		}
		rebuildcommands {
			premakeCommand,
		}

		filter "system:windows"; do
			architecture "x86_64"
		end;
	end
	workspace()
end

return m
