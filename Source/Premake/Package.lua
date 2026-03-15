local Config = require "Config"
local Packages = require "Packages"

local Package = {}

---@alias Package.Projects { [string]: Package.Project }

---@class Package.Project
---@field Name? string
---@field Language? string Language the package is written in. Defaults to "C++"
---@field Kind? string Kind of the package. Defaults to "None" (header only)
---@field PackageDir? string Path to the package source
---@field ProjectDir? string Path to the project file
---@field Source? string[] List of source paths, relative to PackageDir
---@field Include string[] List of include paths, relative to PackageDir
---@field LibDirs? string[] List of library search paths, relative to PackageDir
---@field Libs? string[] List of library names to be linked against
---@field OnProject? fun(package?: Package.Project) Callback run when project is generated. package is nil if GenerateProject is false
---@field OnDepend? fun(package: Package.Project) Callback run inside projects that reference this package

---@class Package.Project.GenerateParams
---@field Packages { [string]: Package.Project}
---@field OnProject? fun(package: Package.Project)

---@param params Package.Project.GenerateParams
function Package.GeneratePackages(params)
	for name, package in spairs(params.Packages) do
		group "Packages"
		Package.GeneratePackageProject(name, package, params.OnProject)
	end
	workspace()
end

---@param name string
---@param package Package.Project
function Package.InitPackageProjectVars(name, package)
	-- Setup any variables not set by the user
	package.Name = package.Name or name
	package.Language = package.Language or premake.CPP
	package.Kind = package.Kind or premake.NONE
	package.PackageDir = package.PackageDir or path.join(Config.PackagesDir, package.Name)
	package.ProjectDir = package.ProjectDir or path.join("%{wks.location}", "Packages", package.Name)

	if Packages[name] and Packages[name].Local then
		package.PackageDir = Packages[name].Local.Path
		if not path.isabsolute(package.PackageDir) then
			package.PackageDir = path.join(Config.RootDir, package.PackageDir)
		end
	end

	-- Package includes should be included in the source to show up properly in IDEs
	package.Source = package.Source or {}

	---@param dirs string[]
	local function forEachAddPackageDir(dirs)
		if not dirs then
			return
		end

		for index, dir in ipairs(dirs) do
			if not path.isabsolute(dir) then
				dirs[index] = path.join(package.PackageDir, dir)
			end
		end
	end

	forEachAddPackageDir(package.Source)
	forEachAddPackageDir(package.Include)
	forEachAddPackageDir(package.LibDirs)
end

---@param name string
---@param package Package.Project
---@param onProject? fun(package: Package.Project)
function Package.GeneratePackageProject(name, package, onProject)
	Package.InitPackageProjectVars(name, package)

	local cwd = os.getcwd()
	os.chdir(package.PackageDir)

	project(package.Name); do
		location(package.ProjectDir)

		if onProject then
			filter {}
			onProject(package)
			filter {}
		end

		-- Remove any files added by onProject
		removefiles { "**" }

		local sourcePatterns = {
			"**.c",
			"**.cpp",
			"**.ixx",
		}
		local headerPatterns = {
			"**.h",
			"**.hpp",
			"**.inc",
			"**.inl",
		}

		-- Add files under source directories to project
		for _, dir in ipairs(package.Source) do
			files(table.translate(
				table.join(sourcePatterns, headerPatterns),
				function(value) return path.join(dir, value) end)
			)
		end

		-- Add files under include directories to project
		for _, dir in ipairs(package.Include) do
			files(table.translate(
				headerPatterns,
				function(value) return path.join(dir, value) end)
			)
		end

		kind(package.Kind)
		language(package.Language)
		warnings "Off"

		includedirs(package.Include)
		libdirs(package.LibDirs)

		filter "platforms:not *Editor*"; do
			if package.Kind == premake.SHAREDLIB then
				kind(premake.STATICLIB)
			end
		end

		filter {}
		if package.OnProject then
			package:OnProject()
		end
		filter {}
	end
	project "*"
	os.chdir(cwd)
end

---@param package Package.Project
function Package.Include(package)
	externalincludedirs(package.Include)

	if (package.LibDirs) then
		libdirs(package.LibDirs)
	end
	if (package.Libs) then
		links(package.Libs)
	end
	if (package.OnDepend) then
		filter {}
		package:OnDepend()
		filter {}
	end
end

---@param callback fun(package, packageName: string, packageDir: string)
local function forEachLocalPackage(callback)
	local localPackages = os.matchdirs(path.join(Config.PackagesDir, "*"))
	for i, packageDir in ipairs(localPackages) do
		if package then
			local packageName = path.getbasename(packageDir)
			local package = Packages[packageName]
			callback(package, packageName, packageDir)
		end
	end
end

---@param optionName string
---@return string
local function packageListTrigger(optionName)
	if _OPTIONS[optionName] then
		if _OPTIONS[optionName] == "" then
			_OPTIONS[optionName] = "all"
		end
		_OPTIONS[optionName] = _OPTIONS[optionName]:lower()
	end
	return optionName
end

newaction {
	trigger = "packages",
	description = "Fetch all packages defined in Packages.lua files",
	execute = function()
		local clean = _OPTIONS["clean"]
		if not clean then
			Package.FetchPackages(Packages)
		else
			forEachLocalPackage(function(package, packageName, packageDir)
				local doClean = clean and (clean == "all" or string.find(clean, ";?" .. packageName:lower() .. ";?"))
				if doClean and os.isdir(packageDir) then
					printf("Cleaning package \"%s\" at %s", packageName, packageDir)
					os.execute(os.translateCommands("{RMDIR} " .. packageDir))
				end
			end)
		end
	end
}

newoption {
	trigger = packageListTrigger("reset"),
	category = "packages",
	value = "<PACKAGE>[;<PACKAGE>...]",
	description = (function()
		local description = "Force fetch the given package(s):\n\tall\n"
		forEachLocalPackage(function(package, packageName, packageDir)
			description = string.format("%s\t%s\n", description, packageName:lower())
		end)
		return description
	end)(),
}

newoption {
	trigger = packageListTrigger("clean"),
	category = "packages",
	value = "<PACKAGE>[;<PACKAGE>...]",
	description = (function()
		local description = "Clean the given package(s):\n\tall\n"
		forEachLocalPackage(function(package, packageName, packageDir)
			description = string.format("%s\t%s\n", description, packageName:lower())
		end)
		return description
	end)(),
}

newoption {
	trigger = "dryrun",
	category = "packages",
	description = "Dry run cleaning and fetching packages",
}

function Package.GeneratePackageCache()
	Package.FetchPackages(Packages)
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
		io.write(("premake packages --reset=%s\n"):format(package.Name:lower()))
		term.popColor()
	else
		if Remote.Url and Remote.Url ~= "" then
			if ref ~= "" then
				printf("Cleaning \"%s\" from %s, Newer Remote Url Specified", package.Name, Remote.Url)
				for _, dir in ipairs(os.matchdirs(path.join(packageDir, "*"))) do
					os.execute(os.translateCommands("{RMDIR} " .. dir))
				end
				for _, file in ipairs(os.matchfiles(path.join(packageDir, "*"))) do
					os.execute(os.translateCommands("{DELETE} " .. file))
				end
			end

			printf("Fetching Archive \"%s\" from %s", package.Name, Remote.Url)
			local result_str, response_code = http.download(Remote.Url, remoteFile)
			if result_str ~= "OK" then error(string.format("[%s] %s", response_code, result_str)) end
		else
			error(("Empty Archive URL Provided for Package \"%s\"!"):format(package.Name))
		end

		printf("\tExtracting Archive File \"%s\" into %s", remoteFile, packageDir)
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
