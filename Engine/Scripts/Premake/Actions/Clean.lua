premake.modules.Clean = {}
local m = premake.modules.Clean

local p = premake

local filesToRemove = {}
local somethingToClean = false

newaction {
    trigger = "clean",
    description = "Deletes the build directory and all project files",

    onStart = function()
        filesToRemove = table.join(
            os.matchfiles("**.sln"),
            os.matchfiles("**.vcxproj*")
        )

        somethingToClean = os.isdir("Build") or not table.isempty(filesToRemove)
    end,
    -- Keep onWorkspace to display completion time at the end
    onWorkspace = function(wks)
    end,
    execute = function()
        if os.isdir("Build") then
            print("Removing Build Directory...")
            os.rmdir("Build")
        end

        for _, file in pairs(filesToRemove) do
            print("Removing " .. file .. "...")
            os.remove(file)
        end
    end,
    onEnd = function()
        if not somethingToClean then
            print("There were no files to clean")
        end
    end,
}

return m