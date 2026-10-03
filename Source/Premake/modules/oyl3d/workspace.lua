local Config = require "Config"

local p = premake

local oyl3d = p.modules.oyl3d
oyl3d.workspace = oyl3d.workspace or {}
oyl3d.generate = oyl3d.generate or {}

local m = oyl3d.workspace
local generate = oyl3d.generate

local private = {}

local package_fetch_table = require(_MAIN_SCRIPT_DIR .. ".Packages")
local package_fetch_file = path.join(_MAIN_SCRIPT_DIR, "Packages.lua")

function oyl3d.wksDotIncludeDir(wks)
	return path.join(wks.basedir, ".Include")
end

function oyl3d.createWksDotIncludeDir(wks)
	local dotIncludeDir = oyl3d.wksDotIncludeDir(wks)

	if os.isdir(dotIncludeDir) then
		return
	end

	-- Make .Include dir and mark the directory as hidden
	os.mkdir(dotIncludeDir)
	if os.host() == premake.WINDOWS then
		os.executef("attrib +h %s /s /d", dotIncludeDir)
	end
end

function private.errorIfPackageNotOnDisk(prj)
	local ok = os.isdir(prj.basedir)
	if not ok then
		term.pushColor(term.errorColor); do
			io.write(('Directory "%s" for Package "%s" not found! Did you run '):format(prj.basedir, prj.name))

			term.pushColor(term.infoColor); do
				io.write("premake packages")
			end
			term.popColor()

			io.write("?\n")
		end
		term.popColor()

		error(err)
	end
end

return m