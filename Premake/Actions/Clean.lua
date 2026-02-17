local Config = require "Config"

premake.modules.Clean = {}
local m = premake.modules.Clean

newoption {
    trigger     = "packages",
    description = "Compatible with 'clean' action. Cleans the package cache.",
    category    = "clean"
}

newaction {
    trigger = "clean",
    description = "Deletes the build directory and all project files",

    onWorkspace = function(wks) -- Keep onWorkspace to display completion time at the end
        local filesToRemove = os.matchfiles(("%s/%s*.sln"):format(wks.location, wks.name))
        for i, v in ipairs(filesToRemove) do
            printf("Removing %s...", v)
            os.remove(v)
        end
    end,
    onProject = function(prj)
        local filesToRemove = table.join(
            os.matchfiles(("%s/%s*.vcxproj*"):format(prj.location, prj.name)),
            os.matchfiles(("%s/%s*.csproj*"):format(prj.location, prj.name))
        )
        for i, v in ipairs(filesToRemove) do
            printf("Removing %s...", v)
            os.remove(v)
        end
    end,
    execute = function()
        if os.isdir("Build") then
            print("Removing Build Directory...")
            os.rmdir("Build")
        end
        
        if _OPTIONS['packages'] then
            local packagesToRemove = os.matchdirs(Config.PackagesDir .. "*")
            for _, package in pairs(packagesToRemove) do
                print("Removing Package '" .. path.getbasename(package) .. "'...")
                os.rmdir(package)
            end
        end
    end,
}

return m