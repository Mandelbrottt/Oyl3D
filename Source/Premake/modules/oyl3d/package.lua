local p = premake

local oyl3d = p.modules.oyl3d
oyl3d.package = oyl3d.package or {}

local m = oyl3d.package
local private = {}

function m.generatePackageProjects(wks)
	function m.generatePackageProjects(wks)
		if type(wks.packageprojects) ~= "table" then
			return
		end

		local package_group = wks.packageprojects.group
		local packages = wks.packageprojects.packages

		for name, package in spairs(packages) do
			local package_fetch_info = Packages[name] -- Grab from "Packages" intentionally
			local package_local_path = os.getcwd()
			if type(package_fetch_info) == "table" then
				package_local_path = package_fetch_info.Local and package_fetch_info.Local.Path or nil
				if not package_local_path then
					package_local_path = path.join(Config.PackageCacheDir, name)
				end
			end

			local cwd = os.getcwd()
			os.chdir(package_local_path)

			group(package_group)
			project(name); do
				local prj = p.api.scope.project

				prj._package = package

				location(path.join(wks.basedir, "Packages", prj.name))
				warnings "Off"

				package.OnProject(prj)
				project(name)

				-- use cwd since it's a cache of the parent cwd
				if prj.basedir:lower() == cwd:lower() then
					error(string.format("Cannot infer basedir for package \"%s\"! Did you forget to call basedir()?",
						name))
				end

				private.errorIfPackageNotOnDisk(prj)

				-- If package has a premake script in the basedir, run it
				local script_dir = prj.basedir
				local script_file = "premake5.lua"
				if os.isfile(path.join(script_dir, script_file)) then
					local cwd = os.getcwd()
					os.chdir(script_dir)

					local script_fn = loadfile(script_file)
					assert(script_fn)()

					-- Included project from script must match name of package
					if p.api.scope.project ~= prj then
						error(
							string.format(
								"Name of project \"%s\" from project script \"%s\" does not including package \"%s\"",
								p.api.scope.project.name,
								p.api.scope.project.script,
								name
							)
						)
					end

					os.chdir(cwd)
				end
			end

			os.chdir(cwd)
		end
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
