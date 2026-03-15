local Actions = require "Actions"
local Config = require "Config"
local Package = require "Package"
local Packages = require "Packages"


premake.modules.Packages = {}
local m = premake.modules.Packages

function m.action()
	newaction(m)

	Actions.SetActionOptions {
		Trigger = m.trigger,
		Keep = {
			"help",
			"debugger",
			"verbose",
			"os",
			"file",
			"scripts",
			"shell",
		},
		Options = m.options
	}
end

m.trigger = "packages"
m.description = "Fetch all packages defined in Packages.lua files"

function m.onStart()
	local index = 1
	while premake.option.list[index] do

	end
end

function m.execute()
	local clean = _OPTIONS["clean"]
	if not clean then
		Package.FetchPackages(Packages)
	else
		m.forEachPackageInCache(function(package, packageName, packageDir)
			local doClean = clean and (clean == "all" or string.find(clean, ";?" .. packageName:lower() .. ";?"))
			if doClean and os.isdir(packageDir) then
				printf("Cleaning package \"%s\" at %s", packageName, packageDir)
				os.execute(os.translateCommands("{RMDIR} " .. packageDir))
			end
		end)
	end
end

function m.options()
	newoption {
		trigger = "reset",
		category = "packages",
		value = "[PACKAGE[;<PACKAGE>...]]",
		description = "Force fetch the given package(s)",
		allowed = (function()
			m.packageListOptionSetDefault("reset", "all")
			local result = { { "all", "Reset all of the packages in the package cache" } }
			m.forEachPackageInCache(function(package, packageName, packageDir)
				table.insert(result, { packageName:lower(), packageDir })
			end)
			return result
		end)(),
	}

	newoption {
		trigger = "clean",
		category = "packages",
		value = "[PACKAGE[;<PACKAGE>...]]",
		description = "Clean the given package(s)",
		allowed = (function()
			m.packageListOptionSetDefault("clean", "all")
			local result = { { "all", "Clean all of the packages in the package cache" } }
			m.forEachPackageInCache(function(package, packageName, packageDir)
				table.insert(result, { packageName:lower(), packageDir })
			end)
			return result
		end)(),
	}

	newoption {
		trigger = "dryrun",
		category = "packages",
		description = "Dry run cleaning and fetching packages",
	}
end

--- Run `callback` on each package in the package cache
---@param callback fun(package, packageName: string, packageDir: string)
function m.forEachPackageInCache(callback)
	-- Match all packages in the package cache
	local packageCache = os.matchdirs(path.join(Config.PackageCacheDir, "*"))

	-- Iterate the package cache and run callback on each package
	for i, packageDir in ipairs(packageCache) do
		local packageName = path.getbasename(packageDir)
		local package = Packages[packageName]
		callback(package, packageName, packageDir)
	end
end

---@param option string
---@param default string
function m.packageListOptionSetDefault(option, default)
	if _OPTIONS[option] then
		if _OPTIONS[option] == "" then
			_OPTIONS[option] = default
		end
		_OPTIONS[option] = _OPTIONS[option]:lower()
	end
end

m.action()

return m
