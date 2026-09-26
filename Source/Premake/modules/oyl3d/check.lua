local Config = require "Config"

local p = premake

local oyl3d = p.modules.oyl3d
oyl3d.check = oyl3d.check or {}
oyl3d.generate = oyl3d.generate or {}

local m = oyl3d.check
local generate = oyl3d.generate
local private = {}

function generate.generateCheckProject(wks)
	if _OPTIONS["no-premake-check"] then
		return
	end

	if not wks.checkproject then
		return
	end

	-- Add link to projects before defining check project to avoid circular dependency
	private.addLinkToAllProjects(wks)
	private.defineCheckProject()
	private.addProjectRegenerateOverrides()
end

function private.addLinkToAllProjects(wks)
	local cwd = os.getcwd()
	for _, prj in ipairs(wks.projects) do
		os.chdir(prj.basedir)

		project(prj.name); do
			links { "Premake" }
		end
	end
	os.chdir(cwd)
end

function private.defineCheckProject()
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

local is_generate_overridden = false

function private.addProjectRegenerateOverrides()
	if not _OPTIONS["premake-check"] then
		return
	end

	if _OPTIONS["no-premake-check"] then
		return
	end

	if is_generate_overridden then
		return
	end

	local isModified = false
	premake.override(premake, "generate", function(base, obj, ext, callback)
		local result = base(obj, ext, callback)
		isModified = isModified or result
		return result
	end)

	premake.override(premake.main, "postAction", function(base)
		base()
		if (isModified and _ACTION:startswith("vs")) then
			printf("One or more project files were regenerated. Exiting with code 1")
			os.exit(1)
		end
	end)
end

return m
