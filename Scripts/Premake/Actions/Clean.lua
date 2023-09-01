premake.modules.Clean = {}
local m = premake.modules.Clean

local p = premake

newoption {
    trigger     = "dependencies",
    description = "Compatible with 'clean' action. Cleans the dependency cache.",
    category    = "clean"
}

local cleanBuildOnly = false

newaction {
    trigger = "clean",
    description = "Deletes the build directory and all project files",

    onStart = function() 
        cleanDependencies = _OPTIONS['dependencies'] ~= nil
    end,
    onWorkspace = function(wks) -- Keep onWorkspace to display completion time at the end
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

        if cleanDependencies then
            local dependenciesToRemove = os.matchdirs(Config.DependenciesDir .. "*")
            for _, dependency in pairs(dependenciesToRemove) do
                print("Removing Dependency '" .. path.getbasename(dependency) .. "'...")
                os.rmdir(dependency)
            end
        end
    end,
}

return m