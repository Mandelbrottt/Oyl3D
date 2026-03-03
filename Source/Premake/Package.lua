local Config = require "Config"
local Engine = require "Engine"

---@class (exact) Package.Archive
---@field Url string

---@class (exact) Package.Git
---@field Url string
---@field Revision? string
---@field Tag? string

---@class Package
---@field GenerateProject? boolean
---@field Name? string
---@field Archive? Package.Archive
---@field Git? Package.Git
---@field Kind? string
---@field Files? string[]
---@field ProjectDir? string Path to the project relative to workspace
---@field IncludeDirs? string[] Paths to include, relative to project
---@field CustomProperties? fun()
---@field DependantProperties? fun(package: Package)

---@type { [string]: Package }
Oyl.Packages = {}

Oyl.Package = {}
local Package = Oyl.Package

function Oyl.Package.SetupPackages()
	for name, package in pairs(Oyl.Packages) do
		Package.SetupVarsInPackage(name, package)
	end
end

---@param name string
---@param package Package
function Package.SetupVarsInPackage(name, package)
	-- Default to Utility if no kind provided
	package.Kind = package.Kind or premake.UTILITY
	package.GenerateProject = package.GenerateProject or true

	package.Name = name
	if (not package.ProjectDir) then
		package.ProjectDir = Config.PackagesDir .. name .. "/"
	end

	if package.IncludeDirs ~= nil then
		for i, includeDir in ipairs(package.IncludeDirs) do
			includeDir = package.ProjectDir .. includeDir
			package.IncludeDirs[i] = includeDir
		end
	else -- If IncludeDirs isn't manually defined, set it to the include folder of the package
		local includeDirsString = package.ProjectDir .. "/**include/"
		package.IncludeDirs = { includeDirsString }
	end
end

function Oyl.Package.GenerateProjects()
	-- Not guaranteed to iterate lua tables in order, sort a proxy array and index into the array
	-- with that
	local orderedKeys = {}
	for k, v in pairs(Oyl.Packages) do
		table.insert(orderedKeys, k)
	end
	table.sort(orderedKeys)
	
	for i = 1, #orderedKeys do
		local package = Oyl.Packages[orderedKeys[i]]
	   
		-- Only generate package projects if an engine project depends on it
		if package.GenerateProject and Engine.DependenciesSet[package.Name] then
			local cwd = os.getcwd()
			os.chdir(package.ProjectDir)

			project(package.Name); do
				Engine.ApplyCommonCppSettings()
				kind(package.Kind)
				includedirs(package.IncludeDirs)
				warnings "Off"

				Engine.FilterStandalone()
				if package.Kind == premake.SHAREDLIB then
					kind(premake.STATICLIB)
				end

				filter {}
				if package['CustomProperties'] then
					package.CustomProperties()
				end
				filter {}

				os.chdir(cwd)
			end
		end
	end
	project "*"
end

newoption {
	trigger = "init-packages",
	category = "packages",
	description = "fetch all packages",
}

newoption {
	trigger = "clean-packages",
	category = "packages",
	description = "Clear and re-fetch selected packages",
	allowed = (function()
		local result = { { "all", "Clean all packages" }, { "", "" } }

		for _, v in ipairs(os.matchdirs(path.join(Config.PackagesDir, "*"))) do
			table.insert(result, { path.getbasename(v):lower(), "" })
		end

		return result
	end)()
}

newoption {
	trigger = "dryrun",
	category = "packages",
	description = "Dry run cleaning and fetching packages",
}

function Package.UpdatePackageCache()
	local reinit = _OPTIONS["clean-packages"] and not _OPTIONS["premake-check"]
	if (reinit) then
		local packageToClean = _OPTIONS["clean-packages"]
		if packageToClean == "all" then
			packageToClean = nil
		end
		Package.CleanPackageCache(packageToClean)
	end
	
	local init = _OPTIONS["init-packages"] or (_ACTION ~= nil and string.sub(_ACTION, 1, 2) == "vs")
	if (init or reinit) then
		Package.FetchPackages(Oyl.Packages)
	end
end

---@param packageToClean? string
function Package.CleanPackageCache(packageToClean)
	local numPackagesCleaned = 0
	for name, package in pairs(Oyl.Packages) do
		-- If directory exists. If packageToClean is specified, also check if the package name matches
		if os.isdir(package.ProjectDir) and (packageToClean == nil or packageToClean:lower() == name:lower()) then
			if (_OPTIONS["dryrun"]) then
				printf("[DRYRUN]\tWould Clean %s...", package.ProjectDir)
			else
				printf("\tCleaning %s...", package.ProjectDir)
				os.execute(os.translateCommands("{RMDIR} " .. package.ProjectDir))
			end
			numPackagesCleaned = numPackagesCleaned + 1
		end
	end
	if numPackagesCleaned == 0 then
		print("No Packages in Cache to remove")
	end
end

---@param packages Package[]
function Oyl.Package.FetchPackages(packages)
	for name, package in pairs(packages) do
		local numFiles = os.match(package.ProjectDir .. "/*")
		if #numFiles == 0 then
			-- For now, trust the user won't add multiple types of packages
			if package.Git then
				Package.GitClone(package)
			end
			if package.Archive then
				Package.FetchArchive(package)
			end
		end
	end
end

local executeOrPrint = function(command)
	local SUPPRESS_COMMAND_OUTPUT = (os.host() == "windows" and "> nul 2>&1" or "> /dev/null 2>&1")
	if (_OPTIONS["verbose"]) then
		SUPPRESS_COMMAND_OUTPUT = ""
	end
	
	if (not _OPTIONS["dryrun"]) then
		term.pushColor(term.infoColor)
		io.write(("\t%s "):format(command))
		term.popColor()
		printf("in %s", os.getcwd())
		os.execute(command .. SUPPRESS_COMMAND_OUTPUT)
	else
		term.pushColor(term.infoColor)
		io.write(("\t%s "):format(command))
		term.popColor()
		printf("in %s", os.getcwd())
	end
end

---@param package Package
function Package.GitClone(package)
	
	local Git = package.Git
	assert(Git ~= nil)

	printf("Cloning Git package \"%s\" from \"%s\"...", package.Name, Git.Url)

	executeOrPrint(string.format("git init %s", package.ProjectDir))

	local cwd = os.getcwd()
	os.chdir(package.ProjectDir)

	executeOrPrint(string.format("git remote add origin %s", Git.Url))

	if package.Files then
		-- Clone specified files and license only
		local files = "!/* /LICENSE*"
		for k, file in pairs(package.Files) do
			if file:sub(1, 1) ~= "/" then
				file = "/" .. file
			end
			files = files .. " " .. file
		end

		executeOrPrint(string.format("git sparse-checkout set --no-cone %s", files))
	end

	-- If a specific revision or tag is specified, point to it
	if Git.Revision then
		local revision = Git.Revision
		printf("\tFetching revision \"%s\"...", revision)
		executeOrPrint(string.format("git fetch -q --depth 1 origin %s", revision))
		executeOrPrint(string.format("git checkout -q --no-progress %s", revision))
	elseif Git.Tag then
		local tag = Git.Tag
		printf("\tFetching tag \"%s\"...", tag)
		executeOrPrint(string.format("git fetch -q --depth 1 origin refs/tags/%s:refs/tags/%s", tag, tag))
		executeOrPrint(string.format("git checkout -q --no-progress %s", tag))
	end

	-- Remove the .git folder as we're only using git to download the repository
	executeOrPrint(os.translateCommands("{RMDIR} .git"))
	os.chdir(cwd)

	term.pushColor(term.green)
	print(string.format("\tPackage \"%s\" cloned succesfully!", package.Name))
	term.popColor()
end

---@param package Package
function Package.FetchArchive(package)
	local Archive = package.Archive
	assert(Archive ~= nil)

	local baseUrl = string.explode(Archive.Url, "?")[1]
	local archiveFile = path.getname(baseUrl)

	os.mkdir(package.ProjectDir)
	local cwd = os.getcwd()
	os.chdir(package.ProjectDir)
	
	if Archive.Url and Archive.Url ~= "" then
		printf("Fetching Archive \"%s\" from %s", package.Name, Archive.Url)
		local result_str, response_code = http.download(Archive.Url, archiveFile)
		if result_str ~= "OK" then error(string.format("[%s] %s", response_code, result_str)) end
	else
		error(("Empty Archive URL Provided for package \"%s\"!"):format(package.Name))
	end

	printf("\tExtracting archive file \"%s\" into %s", archiveFile, package.ProjectDir)
	local tarCommand = ("tar -x -f '%s'"):format(archiveFile)
	if os.host() == premake.WINDOWS then
		executeOrPrint("pwsh -Command " .. tarCommand)
	else
		executeOrPrint(tarCommand)
	end
	os.remove(archiveFile)

	os.chdir(cwd)

	term.pushColor(term.green)
	print(string.format("\tPackage \"%s\" fetched succesfully!", package.Name))
	term.popColor()
end

return Package