local Config = require "Config"

local CheckProject = {}
CheckProject.Name = "Premake"

newoption {
	trigger     = "no-premake-check",
	description = "Disable the automatic run of premake on every compile",
}

---@param additionalArgs? string[]
function CheckProject.GenerateProject(additionalArgs)
	if _OPTIONS["no-premake-check"] then
		return
	end

	---@type any
	local wks = workspace()
	for _, prj in ipairs(wks.projects) do
		project(prj.name); do
			links { CheckProject.Name }
		end
	end

	group "Premake"

	project(CheckProject.Name); do
		kind "Makefile"
		filename("%{prj.name}")
		targetdir(Config.TargetDir .. Config.OutputDir)
		objdir(Config.ObjectDir .. Config.OutputDir)

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

newoption {
	trigger = "premake-check",
	description = "Return a non-zero exit code if any files on disk are modified by the current action",
}

if (_OPTIONS["premake-check"]) then
    local isModified = false
    premake.override(premake, "generate", function(base, obj, ext, callback)
        local result = base(obj, ext, callback)
        isModified = isModified or result
    end)
    
    premake.override(premake.main, "postAction", function(base)
        base()
        if (isModified and _ACTION:sub(1, 2) == "vs") then
            printf("One or more project files were regenerated. Exiting with code 1")
            os.exit(1)
        end
    end)
end

return CheckProject