local Config = require "Config"

local CheckProject = {}
CheckProject.Name = "Premake"

---@param additionalArgs? string[]
function CheckProject.GenerateProject(additionalArgs)
	if _OPTIONS["no-premake-check"] then
		return
	end

	---@type any
	local wks = workspace()
	for _, prj in ipairs(wks.projects) do
		local basedir = prj.basedir
		project(prj.name); do
			prj.blocks[#prj.blocks]._basedir = basedir
			links { CheckProject.Name }
		end
	end

	group "Premake"

	project(CheckProject.Name); do
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

		filter "system:windows"
			architecture "x86_64"
		filter {}
	end
	workspace()
end

return CheckProject