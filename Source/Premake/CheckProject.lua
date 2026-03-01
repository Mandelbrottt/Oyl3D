local Config = require "Config"

Oyl.CheckProject = {}
local CheckProject = Oyl.CheckProject

newoption {
	trigger = "premake-check",
	description = "Return a non-zero exit code if any files on disk are modified by the current action",
}

newoption {
	trigger     = "no-premake-check",
	description = "Disable the automatic run of premake on every compile",
}

---@param args? string[]
function Oyl.CheckProject.GenerateProject(args)
	if _OPTIONS["no-premake-check"] then
		return
	end

	group ""

	project("Premake"); do
		kind "Makefile"
		filename("%{prj.name}_" .. _ACTION)
		targetdir(Config.TargetDir .. Config.OutputDir)
		objdir(Config.ObjectDir .. Config.OutputDir)

		local premakeCommandArgs = {
			_ACTION,
			"--cc=%{cfg.toolset}",
			"--premake-check",
			"--file=" .. _MAIN_SCRIPT
		}
		
		local premakeCommand = "premake5"
		for _, arg in ipairs(premakeCommandArgs) do
			premakeCommand = string.format("%s %s", premakeCommand, arg)
		end
		
		if args then
			for _, arg in ipairs(args) do
				premakeCommand = string.format("%s %s", premakeCommand, arg)
			end
		end

		buildcommands {
			premakeCommand,
		}
		rebuildcommands {
			premakeCommand,
		}

		filter "system:windows"
			architecture "x86_64"
		filter {}
	end
end

return CheckProject