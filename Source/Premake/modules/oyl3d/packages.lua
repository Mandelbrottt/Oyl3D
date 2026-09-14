local p = premake

local oyl3d = p.modules.oyl3d
oyl3d.packages = oyl3d.packages or {}

local m = oyl3d.packages
local private = {}

local opt_clean
local opt_reset

local package_fetch_file = path.join(_MAIN_SCRIPT_DIR, "Packages.lua")
local package_cache_dir = path.join(_MAIN_SCRIPT_DIR, "Packages")

function m.options()
	local reset_trigger = "reset"
	newoption {
		trigger = reset_trigger,
		value = "[PACKAGE[;<PACKAGE>...]]",
		description = "Force fetch the given package(s)",
		allowed = (function()
			if _OPTION[reset_trigger] == "" then
				_OPTION[reset_trigger] = "all"
			end

			local result = { { "all", "Reset all of the packages in the package cache" } }
			private.forEachPackageInCache(function(package, packageName, packageDir)
				table.insert(result, { packageName:lower(), packageDir })
			end)
			return result
		end)(),
	}
	opt_reset = _OPTION[reset_trigger]

	local clean_trigger = "clean"
	newoption {
		trigger = clean_trigger,
		value = "[PACKAGE[;<PACKAGE>...]]",
		description = "Clean the given package(s)",
		allowed = (function()
			if _OPTION[clean_trigger] == "" then
				_OPTION[clean_trigger] = "all"
			end

			local result = { { "all", "Clean all of the packages in the package cache" } }
			private.forEachPackageInCache(function(package, packageName, packageDir)
				table.insert(result, { packageName:lower(), packageDir })
			end)
			return result
		end)(),
	}
	opt_clean = _OPTION[clean_trigger]

	newoption {
		trigger = "dryrun",
		description = "Dry run cleaning and fetching packages",
	}
end

function m.execute()
	if not opt_clean then
		local packageFetchTable = private.getPackageFetchTable()
		private.fetchPackages(packageFetchTable)
	else
		private.forEachPackageInCache(function(package, packageName, packageDir)
			local doClean = opt_clean and (opt_clean == "all" or string.find(opt_clean, ";?" .. packageName:lower() .. ";?"))
			if doClean and os.isdir(packageDir) then
				printf("Cleaning package \"%s\" at %s", packageName, packageDir)
				os.execute(os.translateCommands("{RMDIR} " .. packageDir))
			end
		end)
	end
end

local _package_fetch_table
function private.getPackageFetchTable()
	if not _package_fetch_table then
		_package_fetch_table = require(package_fetch_file)
		assert(type(_package_fetch_table) == "table")
	end
	return _package_fetch_table
end

--- Run `callback` on each package in the package cache
---@param callback fun(package, packageName: string, packageDir: string)
function private.forEachPackageInCache(callback)
	-- Match all packages in the package cache
	local package_cache_dirs = os.matchdirs(path.join(package_cache_dir, "*"))

	local package_fetch_table = private.getPackageFetchTable()

	-- Iterate the package cache and run callback on each package
	for i, package_dir in ipairs(package_cache_dirs) do
		local package_name = path.getbasename(package_dir)
		local package_fetch_info = package_fetch_table[package_name]
		callback(package_fetch_info, package_name, package_dir)
	end
end

function private.fetchPackages(package_fetch_table)
	for package_name, package_fetch_info in spairs(package_fetch_table) do
		local doReset = opt_reset and (opt_reset == "all" or string.find(opt_reset, ";?" .. package_name:lower() .. ";?"))
		local packageDir = path.join(package_cache_dir, package_name)
		if doReset and os.isdir(packageDir) then
			term.pushColor(term.yellow)
			io.write(("Forcing Fetch of Package \"%s\"\n"):format(package_name))
			term.popColor()
			os.execute(os.translateCommands("{RMDIR} " .. packageDir))
		end

		if package_fetch_info.Git then
			private.fetchGit(package_name, package_fetch_info)
		end
		if package_fetch_info.Remote then
			private.fetchRemote(package_name, package_fetch_info)
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

function private.fetchGit(package_name, package_fetch_info)
	local SUPPRESS_COMMAND_OUTPUT = (os.host() == "windows" and "> nul 2>&1" or "> /dev/null 2>&1")
	if (_OPTIONS["verbose"]) then
		SUPPRESS_COMMAND_OUTPUT = ""
	end

	local git = assert(package_fetch_info.Git)

	local package_dir = path.join(package_cache_dir, package_name)

	if not os.isdir(package_dir) then
		executef("git init %s %s", package_dir, SUPPRESS_COMMAND_OUTPUT)
	end

	local cwd = os.getcwd()
	os.chdir(package_dir)

	local currentSha, _ = os.outputof("git rev-parse HEAD")
	local currentBranch, _ = os.outputof("git rev-parse --abbrev-ref HEAD")
	local currentTag, _ = os.outputof("git describe --tags --exact-match HEAD")
	local currentRef = ""
	currentRef = currentSha == git.Ref and currentSha or currentRef
	currentRef = currentBranch == git.Ref and currentBranch or currentRef
	currentRef = currentTag == git.Ref and currentTag or currentRef

	-- If already on ref, do nothing
	if git.Ref == currentRef then
		term.pushColor(term.green)
		printf("Git Package \"%s\" up-to-date on Ref %s", package_name, git.Ref)
		term.popColor()
		return
	end

	local remote, _ = os.outputof("git remote -v")
	if not remote or remote == "" then
		executef("git remote add origin %s %s", git.Url, SUPPRESS_COMMAND_OUTPUT)
		executef("git config --local gc.auto 0 %s", SUPPRESS_COMMAND_OUTPUT)
		executef("git config --local advice.detachedHead false %s", SUPPRESS_COMMAND_OUTPUT)
	end

	if git.Sparse then
		local patterns = "!/* /LICENSE*"
		for k, pattern in pairs(git.Sparse) do
			patterns = patterns .. " " .. pattern
		end
		executef("git sparse-checkout set --no-cone %s", patterns)
	else
		executef("git sparse-checkout disable")
	end

	-- If no ref is specified, get the head revision
	git.Ref = git.Ref or "HEAD"

	-- If a ref is specified, clone it
	printf("Cloning Git package \"%s\" from \"%s\"...", package_name, git.Url)
	printf("\tFetching Ref \"%s\"...", git.Ref)

	local fetchCommand = table.implode({
		"git fetch",
		"--no-tags --prune --recurse-submodules --depth 1",
		"origin",
		"+{REF}",
		"+refs/heads/{REF}*:refs/remotes/origin/{REF}*",
		"+refs/tags/{REF}*:refs/tags/{REF}*"
	}, "", "", " ")
	fetchCommand = fetchCommand:gsub("%{REF%}", git.Ref)
	executef("%s %s", fetchCommand, SUPPRESS_COMMAND_OUTPUT)

	local branch, _ = os.outputof(string.format("git branch --list --remote origin/%s", git.Ref))
	local tag, _ = os.outputof(string.format("git tag --list %s", git.Ref))

	local ref = ""
	if branch ~= nil and branch ~= "" then
		ref = "refs/remotes/origin/" .. branch
	elseif tag ~= nil and tag ~= "" then
		ref = "refs/tags/" .. tag
	else
		ref = git.Ref
	end

	executef("git checkout --progress --force --quiet %s", ref)
	executef("git submodule update --init --recursive")

	os.chdir(cwd)

	term.pushColor(term.green)
	print(string.format("\tPackage \"%s\" cloned succesfully!", package_name))
	term.popColor()
end

function private.fetchRemote(package_name, package_fetch_info)
	local remote = assert(package_fetch_info.Remote)

	local baseUrl = string.explode(remote.Url, "?")[1]
	local remoteFile = path.getname(baseUrl)

	local package_dir = path.join(package_cache_dir, package_name)

	os.mkdir(package_dir)
	local cwd = os.getcwd()
	os.chdir(package_dir)

	local refFile = ".ref"
	local ref = ""
	if os.isfile(refFile) then
		ref = io.readfile(refFile)
	end

	-- If ref is currently downloaded, don't fetch
	if ref == remote.Url then
		term.pushColor(term.green)
		printf("Remote Package \"%s\" up-to-date with URL \"%s\"", package_name, remote.Url)
		term.popColor()
		io.write("\tIf you know the remote file has changed, consider running ")
		term.pushColor(term.infoColor)
		io.write(("premake packages --reset=%s\n"):format(package_name:lower()))
		term.popColor()
	else
		if remote.Url and remote.Url ~= "" then
			if ref ~= "" then
				printf("Cleaning \"%s\" from %s, Newer Remote Url Specified", package_name, remote.Url)
				for _, dir in ipairs(os.matchdirs(path.join(package_dir, "*"))) do
					os.execute(os.translateCommands("{RMDIR} " .. dir))
				end
				for _, file in ipairs(os.matchfiles(path.join(package_dir, "*"))) do
					os.execute(os.translateCommands("{DELETE} " .. file))
				end
			end

			printf("Fetching Archive \"%s\" from %s", package_name, remote.Url)
			local result_str, response_code = http.download(remote.Url, remoteFile)
			if result_str ~= "OK" then error(string.format("[%s] %s", response_code, result_str)) end
		else
			error(("Empty Archive URL Provided for Package \"%s\"!"):format(package_name))
		end

		printf("\tExtracting Archive File \"%s\" into %s", remoteFile, package_dir)
		local tarCommand = ("tar -x -f '%s'"):format(remoteFile)
		if os.host() == premake.WINDOWS then
			executef("pwsh -Command " .. tarCommand)
		else
			executef(tarCommand)
		end

		os.remove(remoteFile)
		io.writefile(refFile, remote.Url)

		term.pushColor(term.green)
		print(string.format("\tPackage \"%s\" fetched successfully!", package_name))
		term.popColor()
	end

	os.chdir(cwd)
end

return m
