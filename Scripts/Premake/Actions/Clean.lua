premake.modules.Clean = {}
local m = premake.modules.Clean

local p = premake

newoption {
    trigger = "build",
    description = "Compatible with 'clean' action. Cleans only the build directory."
}

local cleanBuildOnly = false

newaction {
    trigger = "clean",
    description = "Deletes the build directory and all project files",

    onStart = function() 
        cleanBuildOnly = _OPTIONS['build'] ~= nil
    end,
    onWorkspace = function(wks) -- Keep onWorkspace to display completion time at the end
    end,
    execute = function()
        if os.isdir("Build") then
            print("Removing Build Directory...")
            os.rmdir("Build")
        end

        if cleanBuildOnly then
            return
        end

        local filesToRemove = table.join(
            os.matchfiles("**.sln"),
            os.matchfiles("**.vcxproj*")
        )

        for _, file in pairs(filesToRemove) do
            print("Removing " .. file .. "...")
            os.remove(file)
        end
    end,
}

return m