premake.modules.Build = {}
local m = premake.modules.Build

newaction {
    trigger = "build",
    description = "Builds the project based on selected toolset",

    -- Keep onWorkspace to display completion time at the end
    onWorkspace = function(wks)
    end,
    execute = function()
        term.pushColor(term.red)
        print("Not yet implemented")
        term.popColor()

        -- https://stackoverflow.com/questions/328017/path-to-msbuild
    end,
}

return m