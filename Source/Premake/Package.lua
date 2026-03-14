local Packages = require "Packages"

local Config = require "Config"

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
---@field IncludeDirs? string[] List of include Paths, relative to project
---@field LibDirs? string[] List of library Search Paths, relative to project
---@field Libs? string[] List of library names to be linked against, in LibDirs
---@field OnFetch? fun(package: Package) Callback run once the package has been fetched and is on disk, before project generation
---@field CustomProperties? fun(package?: Package) Callback run when project is generated. package is nil if GenerateProject is false
---@field DependantProperties? fun(package: Package) Callback run inside projects that reference this package

Oyl.Package = {}
local Package = Oyl.Package

newaction {
	trigger = "packages",
	description = "Fetch all packages defined in Packages.lua files",
	execute = function()
		Package.FetchPackages(Packages)
	end
}

newoption {
	trigger = "dryrun",
	category = "packages",
	description = "Dry run cleaning and fetching packages",
}

function Package.GeneratePackageCache()
	Package.FetchPackages(Packages)
end

---@param package Package
---@return boolean is the package fetchable? If false, package is expected to already be on disk (SDKs)
function Package.IsFetchable(package)
	return package.Git ~= nil or package.Archive ~= nil
end

newoption {
	trigger = "reset",
	category = "packages",
	value = "<PACKAGE>[;<PACKAGE>...]",
	description = (function()
		local description = "Force fetch the given package(s)"
		local localPackages = os.matchdirs(path.join(Config.PackagesDir, "*"))
		if #localPackages == 0 then
			return description
		end
		description = description .. ":\n\tall\n"
		for i, v in ipairs(localPackages) do
			local packageName = path.getbasename(v)
			local package = Packages[packageName]
			if package then
				description = string.format("%s\t%s\n", description, packageName:lower())
			end
		end
		return description
	end)(),
}

if _OPTIONS["reset"] then
	if _OPTIONS["reset"] == "" then
		_OPTIONS["reset"] = "all"
	end
	_OPTIONS["reset"] = _OPTIONS["reset"]:lower()
end

---@param packages Packages
function Package.FetchPackages(packages)
	for name, package in spairs(packages) do
		package.Name = name

		local reset = _OPTIONS["reset"]
		local doReset = reset and (reset == "all" or string.find(reset, ";?" .. name:lower() .. ";?"))
		local packageDir = path.join(Config.PackagesDir, name)
		if doReset and os.isdir(packageDir) then
			term.pushColor(term.yellow)
			io.write(("Forcing Fetch of Package \"%s\"\n"):format(name))
			term.popColor()
			os.execute(os.translateCommands("{RMDIR} " .. packageDir))
		end

		if package.Git then
			Package.FetchGit(package)
		end
		if package.Remote then
			Package.FetchRemote(package)
		end
	end
end

function Oyl.Package.SetupPackages()
	for name, package in pairs(Oyl.Packages) do
		Package.SetupVarsInPackage(name, package)
	end
end

---@param name string
---@param package Package
function Package.SetupVarsInPackage(name, package)
	-- Default to None if no kind provided
	package.Kind = package.Kind or premake.NONE
	if package.GenerateProject == nil then
		package.GenerateProject = true
	end

	package.Name = name
	if (not package.ProjectDir) then
		package.ProjectDir = Config.PackagesDir .. name .. "/"
	end

	-- Add project directory to include directories
	if package.IncludeDirs ~= nil then
		for i, includeDir in ipairs(package.IncludeDirs) do
			includeDir = path.join(package.ProjectDir, includeDir)
			package.IncludeDirs[i] = includeDir
		end
	else -- If IncludeDirs isn't manually defined, set it to the include folder of the package
		local includeDirsString = path.join(package.ProjectDir, "**include")
		package.IncludeDirs = { includeDirsString }
	end

	-- Add project directory to lib directories
	if package.LibDirs ~= nil then
		for i, libDir in ipairs(package.LibDirs) do
			libDir = path.join(package.ProjectDir, libDir)
			package.LibDirs[i] = libDir
		end
	end
end

-- function Oyl.Package.GenerateProjects()
-- 	-- Not guaranteed to iterate lua tables in order, sort a proxy array and index into the array
-- 	-- with that
-- 	local orderedKeys = {}
-- 	for k, v in pairs(Oyl.Packages) do
-- 		table.insert(orderedKeys, k)
-- 	end
-- 	table.sort(orderedKeys)

-- 	for i = 1, #orderedKeys do
-- 		local package = Oyl.Packages[orderedKeys[i]]

-- 		local cwd = os.getcwd()
-- 		os.chdir(package.ProjectDir)

-- 		-- Generate package project if specified, otherwise just run CustomProperties callback
-- 		-- This allows clients to do custom processing on the worktree once the package has been fetched
-- 		if package.GenerateProject and Engine.DependenciesSet[package.Name] then
-- 			project(package.Name); do
-- 				Engine.ApplyCommonCppSettings()
-- 				kind(package.Kind)
-- 				includedirs(package.IncludeDirs)
-- 				warnings "Off"

-- 				Engine.FilterStandalone()
-- 				if package.Kind == premake.SHAREDLIB then
-- 					kind(premake.STATICLIB)
-- 				end

-- 				filter {}
-- 				if package.CustomProperties then
-- 					package:CustomProperties()
-- 				end
-- 				filter {}
-- 			end
-- 		end

-- 		os.chdir(cwd)
-- 	end
-- 	project "*"
-- end

-- ---@param package Package
-- function Oyl.Package.Include(package)
-- 	if (package.GenerateProject and package.Kind ~= premake.NONE) then
-- 		links { package.Name }
-- 	end
-- 	if (package.LibDirs) then
-- 		libdirs(package.LibDirs)
-- 	end
-- 	if (package.Libs) then
-- 		links(package.Libs)
-- 	end
-- 	if (package.IncludeDirs) then
-- 		externalincludedirs(package.IncludeDirs)
-- 	end
-- 	if (package.DependantProperties) then
-- 		package:DependantProperties()
-- 		filter {}
-- 	end
-- end

local executef = function(command, ...)
	command = string.format(command, ...)

	if (not _OPTIONS["dryrun"]) then
		if _OPTIONS["verbose"] then
			term.pushColor(term.infoColor)
			io.write(("\t%s "):format(command))
			term.popColor()
			printf("in %s", os.getcwd())
		end
		os.execute(command)
	else
		term.pushColor(term.infoColor)
		io.write(("\t%s "):format(command))
		term.popColor()
		printf("in %s", os.getcwd())
	end
end

---@param package Packages.Definition
function Package.FetchGit(package)
	local SUPPRESS_COMMAND_OUTPUT = (os.host() == "windows" and "> nul 2>&1" or "> /dev/null 2>&1")
	if (_OPTIONS["verbose"]) then
		SUPPRESS_COMMAND_OUTPUT = ""
	end
	
	local Git = package.Git
	assert(Git ~= nil)

	local packageDir = path.join(Config.PackagesDir, package.Name)

	if not os.isdir(packageDir) then
		executef("git init %s %s", packageDir, SUPPRESS_COMMAND_OUTPUT)
	end

	local cwd = os.getcwd()
	os.chdir(packageDir)

	local currentSha, _ = os.outputof("git rev-parse HEAD")
	local currentBranch, _ = os.outputof("git rev-parse --abbrev-ref HEAD")
	local currentTag, _ = os.outputof("git describe --tags --exact-match HEAD")
	local currentRef = ""
	currentRef = currentSha == Git.Ref and currentSha or currentRef
	currentRef = currentBranch == Git.Ref and currentBranch or currentRef
	currentRef = currentTag == Git.Ref and currentTag or currentRef

	-- If already on ref, do nothing
	if Git.Ref == currentRef then
		term.pushColor(term.green)
		printf("Git Package \"%s\" up-to-date on Ref %s", package.Name, Git.Ref)
		term.popColor()
		return
	end

	local remote, _ = os.outputof("git remote -v")
	if not remote or remote == "" then
		executef("git remote add origin %s %s", Git.Url, SUPPRESS_COMMAND_OUTPUT)
		executef("git config --local gc.auto 0 %s", SUPPRESS_COMMAND_OUTPUT)
		executef("git config --local advice.detachedHead false %s", SUPPRESS_COMMAND_OUTPUT)
		
	end

	-- If no ref is specified, get the head revision
	Git.Ref = Git.Ref or "HEAD"

	-- If a ref is specified, clone it
	printf("Cloning Git package \"%s\" from \"%s\"...", package.Name, Git.Url)
	printf("\tFetching Ref \"%s\"...", Git.Ref)

	local fetchCommand = table.implode({
		"git fetch",
		"--no-tags --prune --no-recurse-submodules --depth 1",
		"origin",
		"+{REF}",
		"+refs/heads/{REF}*:refs/remotes/origin/{REF}*",
		"+refs/tags/{REF}*:refs/tags/{REF}*"
	}, "", "", " ")
	fetchCommand = fetchCommand:gsub("%{REF%}", Git.Ref)
	executef("%s %s", fetchCommand, SUPPRESS_COMMAND_OUTPUT)

	local branch, _ = os.outputof(string.format("git branch --list --remote origin/%s", Git.Ref))
	local tag, _ = os.outputof(string.format("git tag --list %s", Git.Ref))

	local ref = ""
	if branch ~= nil and branch ~= "" then
		ref = "refs/remotes/origin/" .. branch
	elseif tag ~= nil and tag ~= "" then
		ref = "refs/tags/" .. tag
	else
		ref = Git.Ref
	end

	executef("git checkout --progress --force --quiet %s", ref)

	os.chdir(cwd)

	term.pushColor(term.green)
	print(string.format("\tPackage \"%s\" cloned succesfully!", package.Name))
	term.popColor()
end

---@param package Packages.Definition
function Package.FetchRemote(package)
	local Remote = package.Remote
	assert(Remote ~= nil)

	local baseUrl = string.explode(Remote.Url, "?")[1]
	local remoteFile = path.getname(baseUrl)

	local packageDir = path.join(Config.PackagesDir, package.Name)

	os.mkdir(packageDir)
	local cwd = os.getcwd()
	os.chdir(packageDir)

	local refFile = ".ref"
	local ref = ""
	if os.isfile(refFile) then
		ref = io.readfile(refFile)
	end

	-- If ref is currently downloaded, don't fetch
	if ref == Remote.Url then
		term.pushColor(term.green)
		printf("Remote Package \"%s\" up-to-date with URL \"%s\"", package.Name, Remote.Url)
		term.popColor()
		io.write("\tIf you know the remote file has changed, consider running ")
		term.pushColor(term.infoColor)
		io.write(("premake packages --reset=%s\n"):format(package.Name))
		term.popColor()
	else
		if Remote.Url and Remote.Url ~= "" then
			printf("Fetching Archive \"%s\" from %s", package.Name, Remote.Url)
			local result_str, response_code = http.download(Remote.Url, remoteFile)
			if result_str ~= "OK" then error(string.format("[%s] %s", response_code, result_str)) end
		else
			error(("Empty Archive URL Provided for package \"%s\"!"):format(package.Name))
		end

		printf("\tExtracting archive file \"%s\" into %s", remoteFile, packageDir)
		local tarCommand = ("tar -x -f '%s'"):format(remoteFile)
		if os.host() == premake.WINDOWS then
			executef("pwsh -Command " .. tarCommand)
		else
			executef(tarCommand)
		end

		os.remove(remoteFile)
		io.writefile(refFile, Remote.Url)

		term.pushColor(term.green)
		print(string.format("\tPackage \"%s\" fetched succesfully!", package.Name))
		term.popColor()
	end

	os.chdir(cwd)
end

return Package
