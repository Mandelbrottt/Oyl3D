premake.modules.Build = {}
local m = premake.modules.Build

function m.action()
	newaction(m)
end

m.trigger = "build"
m.description = "Builds the project based on selected toolset"

function m.onWorkspace(wks)

end

function m.execute()
	term.pushColor(term.red)
	print("Not yet implemented")
	term.popColor()

	-- https://stackoverflow.com/questions/328017/path-to-msbuild
end

m.action()

return m
