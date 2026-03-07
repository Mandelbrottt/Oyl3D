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
		local filesToRemove = table.join(
			os.matchfiles(("%s/%s*.sln"):format(wks.location, wks.name)),
			os.matchfiles(("%s/Makefile"):format(wks.location)),
			os.matchfiles(("%s/%s*.make"):format(wks.location, wks.name)),
			os.matchfiles(("%s/%s*.ninja"):format(wks.location, wks.name))
		)
		for _, v in ipairs(filesToRemove) do
			printf("Removing %s...", path.getrelative(os.getcwd(), v))
			os.remove(v)
		end
	end,
	onProject = function(prj)
		if _OPTIONS['packages'] and string.contains(prj.location:lower(), Config.PackagesDir:lower()) then
			print(("Removing Package '%s'..."):format(prj.name))
			os.rmdir(prj.location)
		else
			local filesToRemove = table.join(
				os.matchfiles(("%s/%s*.vcproj*"):format(prj.location, prj.name)),
				os.matchfiles(("%s/%s*.vcxproj*"):format(prj.location, prj.name)),
				os.matchfiles(("%s/%s*.csproj*"):format(prj.location, prj.name)),
				os.matchfiles(("%s/Makefile"):format(prj.location)),
				os.matchfiles(("%s/%s*.make"):format(prj.location, prj.name)),
				os.matchfiles(("%s/%s*.ninja"):format(prj.location, prj.name))
			)
			for _, v in ipairs(filesToRemove) do
				printf("Removing %s...", path.getrelative(os.getcwd(), v))
				os.remove(v)
			end
		end
	end,
	execute = function()
		if os.isdir("Build") then
			print("Removing Build Directory...")
			os.rmdir("Build")
		end
	end,

}

return m