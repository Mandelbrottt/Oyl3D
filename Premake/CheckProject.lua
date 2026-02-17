local Config = require "Config"

Oyl.CheckProject = {}
local CheckProject = Oyl.CheckProject

newoption {
    trigger     = "no-premake-check",
    description = "Disable the automatic run of premake on every compile",
}

function Oyl.CheckProject.Generate()
    if _OPTIONS["no-premake-check"] then
        return
    end

    project("Premake")
    do
        kind "Makefile"
        filename("%{prj.name}_" .. _ACTION)
        targetdir(Config.TargetDir .. Config.OutputDir)
        objdir(Config.ObjectDir .. Config.OutputDir)

        local runPremakeCommand = ("%s %s"):format(
            "%{wks.location}/Binaries/ThirdParty/premake5.exe ",
            table.concat(_ARGV, " ")
        )

        if (_ARGV[#_ARGV] ~= "--error-on-generate") then
            runPremakeCommand = ("%s %s"):format(runPremakeCommand, "--error-on-generate")
        end

        buildcommands {
            runPremakeCommand,
        }
        rebuildcommands {
            runPremakeCommand,
        }

        filter "system:windows"
            architecture "x86_64"
        filter {}
    end
end

return CheckProject