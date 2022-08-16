premake.modules.Clean = {}
local m = premake.modules.Clean

local p = premake

newaction {
    trigger = "clean",
    description = "Deletes the build directory and all project files",

    -- Keep onWorkspace to display completion time at the end
    onWorkspace = function(wks)
    end,
    execute = function()
        if os.isdir("Build") then
            print("Removing Build Directory...")
            os.rmdir("Build")
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