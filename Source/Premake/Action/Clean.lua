local Actions = require "Actions"
local Config = require "Config"

premake.modules.Clean = {}
local m = premake.modules.Clean

function m.action()
	newaction(m)

	Actions.SetActionOptions {
		Trigger = m.trigger,
		Keep = {
			"help",
			"debugger",
			"verbose",
			"file",
			"scripts",
		},
		Options = m.options
	}
end

m.trigger = "clean"
m.description = "Deletes the build directory and all project files"

function m.onWorkspace(wks)
	local filesToRemove = table.join(
		os.matchfiles(("%s/%s*.sln"):format(wks.location, wks.name)),
		os.matchfiles(("%s/Makefile"):format(wks.location)),
		os.matchfiles(("%s/%s*.make"):format(wks.location, wks.name)),
		os.matchfiles(("%s/%s*.ninja"):format(wks.location, wks.name))
	)
	for _, file in ipairs(filesToRemove) do
		printf("Cleaning %s...", path.getrelative(os.getcwd(), file))
		os.remove(file)
	end
end

function m.onProject(prj)
	if _OPTIONS['packages'] and string.contains(prj.location:lower(), Config.PackageCacheDir:lower()) then
		print(("Cleaning Package '%s'..."):format(prj.name))
		os.rmdir(prj.location)
	end

	local filesToRemove = table.join(
		os.matchfiles(("%s/%s*.vcproj*"):format(prj.location, prj.name)),
		os.matchfiles(("%s/%s*.vcxproj*"):format(prj.location, prj.name)),
		os.matchfiles(("%s/%s*.csproj*"):format(prj.location, prj.name)),
		os.matchfiles(("%s/Makefile"):format(prj.location)),
		os.matchfiles(("%s/%s*.make"):format(prj.location, prj.name)),
		os.matchfiles(("%s/%s*.ninja"):format(prj.location, prj.name))
	)
	for _, file in ipairs(filesToRemove) do
		printf("Cleaning %s...", path.getrelative(os.getcwd(), file))
		os.remove(file)
	end
end

function m.execute()
	if os.isdir("Build") then
		print("Cleaning Build Directory...")
		os.rmdir("Build")
	end
end

function m.options()
	newoption {
		trigger     = "packages",
		description = "Clean the package cache",
		category    = m.trigger
	}
end

m.action()

return m
