local p = premake

local oyl3d = p.modules.oyl3d
oyl3d.packages = oyl3d.packages or {}

local m = oyl3d.packages
local private = {}

local opt_fetch
local opt_reset

m.skip_fetch_actions = {
	"clean"
}

premake.override(p.main, "processCommandLine", function(base)
	m.options()
	base()
end)

function m.options()
	local options_category = "Oyl3D"

	local reset_trigger = "resetpackages"
	newoption {
		trigger = reset_trigger,
		value = "[PACKAGE[;<PACKAGE>...]]",
		description = "Force fetch the given package(s)",
		category = options_category,
		allowed = (function()
			if _OPTIONS[reset_trigger] == "" then
				_OPTIONS[reset_trigger] = "all"
			end

			local result = { { "all", "Reset all of the packages in the package cache" } }
			for wks in p.global.eachWorkspace() do
				if not wks.packageprojects then
					return
				end

				local packages = wks.packageprojects.packages
				local cachedir = path.join(wks.basedir, wks.packageprojects.cachedir or "Packages")
				for name, package in spairs(packages) do
					table.insert(result, { name:lower(), cachedir })
				end
			end
			return result
		end)(),
	}
	opt_reset = _OPTIONS[reset_trigger]

	local fetch_trigger = "fetchpackages"
	newoption {
		trigger = fetch_trigger,
		description = "Fetch packages for all workspaces",
		category = options_category,
	}
	opt_fetch = _OPTIONS[fetch_trigger]

	newoption {
		trigger = "dryrun",
		description = "Dry run cleaning and fetching packages",
		category = options_category,
	}
end

function m.fetch(package_name, package_table, cachedir)
	for _, action in ipairs(m.skip_fetch_actions) do
		if _ACTION == action then
			return
		end
	end

	local package_dir = path.join(cachedir, package_name)

	local do_reset = opt_reset and
		(opt_reset == "all" or string.find(opt_reset, ";?" .. package_name:lower() .. ";?"))
	if do_reset and os.isdir(package_dir) then
		term.pushColor(term.yellow)
		io.write(("Forcing Fetch of Package \"%s\"\n"):format(package_name))
		term.popColor()
		os.execute(os.translateCommands("{RMDIR} " .. package_dir))
	end

	if not opt_fetch then
		-- If the user hasn't specifically requested a fetch, only fetch if the package isn't on disk
		if os.isdir(package_dir) then
			return
		end
	end

	if package_table.Git then
		private.fetchGit(package_name, package_table, package_dir)
	end
	if package_table.Remote then
		private.fetchRemote(package_name, package_table, package_dir)
	end
	if package_table.Local then
		private.fetchLocal(package_name, package_table, package_dir)
	end
end

function m.clean(package_name, cache_dir)
	local package_dir = path.join(cache_dir, package_name)
	if os.isdir(package_dir) then
		os.rmdir(package_dir)
	end
end

--- Run `callback` on each package in the package cache
---@param wks table
---@param callback fun(package, packageName: string, packageDir: string)
function private.forEachPackageInCache(wks, callback)
	print(wks.name)
	if not wks.packageprojects then
		return
	end

	local package_fetch_table = wks.packageprojects.packages or {}
	local package_cache_dir = path.join(wks.basedir, wks.packageprojects.cachedir or "Packages")

	-- Match all packages in the package cache
	local package_cache_dirs = os.matchdirs(path.join(package_cache_dir, "*"))

	-- Iterate the package cache and run callback on each package
	for i, package_dir in ipairs(package_cache_dirs) do
		print(package_name)
		local package_name = path.getbasename(package_dir)
		local package_fetch_info = package_fetch_table[package_name]
		callback(package_fetch_info, package_name, package_dir)
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

function private.fetchGit(package_name, package_fetch_info, package_dir)
	local SUPPRESS_COMMAND_OUTPUT = (os.host() == "windows" and "> nul 2>&1" or "> /dev/null 2>&1")
	if (_OPTIONS["verbose"]) then
		SUPPRESS_COMMAND_OUTPUT = ""
	end

	local git = assert(package_fetch_info.Git)

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
	printf("\tPackage \"%s\" cloned succesfully!", package_name)
	term.popColor()
end

function private.fetchRemote(package_name, package_fetch_info, package_dir)
	local remote = assert(package_fetch_info.Remote)

	local baseUrl = string.explode(remote.Url, "?")[1]
	local remoteFile = path.getname(baseUrl)

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
		io.write(("premake <action> --resetpackages=%s\n"):format(package_name:lower()))
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
			if result_str ~= "OK" then p.error("[%s] %s", response_code, result_str) end
		else
			p.error("Empty Archive URL Provided for Package \"%s\"!", package_name)
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
		printf("\tRemote Package \"%s\" fetched successfully!", package_name)
		term.popColor()
	end

	os.chdir(cwd)
end

function private.fetchLocal(package_name, package_fetch_info, package_dir)
	local local_path = assert(package_fetch_info.Local and package_fetch_info.Local.Path)

	if os.isdir(local_path) then
		term.pushColor(term.green)
		printf("Local Package \"%s\" found at \"%s\"!", package_name, local_path)
		term.popColor()
	else
		p.error("Local Package \"%s\" not found at \"%s\"!", package_name, local_path)
	end
end

return m
