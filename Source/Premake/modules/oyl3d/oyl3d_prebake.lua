local p = premake

local oyl3d = p.modules.oyl3d
oyl3d.preBake = oyl3d.preBake or {}
oyl3d.packages = oyl3d.packages or {}

local m = oyl3d.preBake
local private = {}

function m.onWorkspaceModuleProjects(wks)
	workspace(wks.name)

	-- Recurse through the workspace directory and include all premake scripts
	local premake_script_pattern = "*/**/premake5.lua"
	local script_paths = os.matchfiles(premake_script_pattern)

	-- Invoke Project Scripts
	for _, script_path in ipairs(script_paths) do
		local script_dir = path.getdirectory(script_path)
		local script_file = path.getname(script_path)

		local cwd = os.getcwd()
		os.chdir(script_dir)

		local script_fn = loadfile(script_file)
		assert(script_fn)()

		os.chdir(cwd)
	end
end

function m.onWorkspacePackageProjects(wks)
	if type(wks.packageprojects) ~= "table" then
		return
	end

	workspace(wks.name)

	local package_group = wks.packageprojects.group
	local packages = wks.packageprojects.packages
	local cachedir = path.join(wks.basedir, wks.packageprojects.cachedir or "Packages")

	for name, package in spairs(packages) do
		oyl3d.packages.fetch(name, package, cachedir)

		local package_fetch_info = packages[name]
		local package_local_path = path.join(cachedir, name)
		if type(package_fetch_info) == "table" and package_fetch_info.Local then
			package_local_path = package_fetch_info.Local.Path
		end

		local cwd = os.getcwd()
		local chdir_ok, chdir_err = os.chdir(package_local_path)

		if not chdir_ok then
			os.chdir(cwd)
			return
		end

		group(package_group)
		project(name); do
			local prj = p.api.scope.project

			prj._package = package

			location(path.join(cachedir, name))
			warnings "Off"

			if type(package.OnProject) == "function" then
				package.OnProject(prj)
				project(name)
			end

			-- If package has a premake script in the basedir, run it
			local script_dir = prj.basedir
			local script_file = "premake5.lua"
			local cwd = os.getcwd()
			os.chdir(script_dir)

			if os.isfile(path.join(script_dir, script_file)) then
				local script_fn = loadfile(script_file)
				assert(script_fn)()

				-- Included project from script must match name of package
				if p.api.scope.project ~= nil and p.api.scope.project ~= prj then
					error(
						string.format(
							"Name of project \"%s\" from project script \"%s\" does not including package \"%s\"",
							p.api.scope.project.name,
							p.api.scope.project.script,
							name
						)
					)
				end
			end

			os.chdir(cwd)
		end

		os.chdir(cwd)
	end
end

function m.onProjectApplyDefaults(prj)
	local wks = prj.workspace
	local defaults = wks.projectdefaults
	if not defaults then
		return
	end

	local nBlocks = #prj.blocks

	local default_func
	for language, func in pairs(defaults) do
		if prj.language:lower() == language:lower() then
			default_func = func
		end
	end

	if not default_func then
		return
	end

	-- Force a new block
	project(prj.name)
	default_func()

	-- Move all newly created blocks to the beginning of the blocks list
	if nBlocks < #prj.blocks then
		-- For some reason in-place table.move isn't working, so use a middleman table
		local new_blocks = table.move(prj.blocks, nBlocks + 1, #prj.blocks, 1, {})
		table.move(prj.blocks, 1, nBlocks, #new_blocks + 1, new_blocks)
		prj.blocks = new_blocks
	end
	-- Force a new block since we did some shenanigans
	project(prj.name)
end

function m.onProjectApplySharedToStaticLib(prj)
	if prj.lockkind then
		return
	end

	if prj.kind ~= "SharedLib" then
		return
	end

	filter { "platforms:Standalone" }; do
		kind "StaticLib"
	end;
end

function m.onProjectModuleProjectUsages(prj)
	vpaths {
		["Public/*"] = path.join("Public/", prj.name, "**"),
		["Private/*"] = path.join("Private/", "**")
	}
	usage "PRIVATE"; do
		includedirs {
			"Private",
			"Public",
		}
	end
	usage "INTERFACE"; do
		links { prj.name }
		includedirsafter {
			"Public"
		}
	end
end

function m.onProjectDefineModuleMacros(prj)
	local prj_name_as_macro = prj.name:upper():gsub("[%.%-% ]", "_")

	defines {
		"OYL_MODULE_WITHIN_" .. prj_name_as_macro,
		"OYL_MODULE_CURRENT=\"" .. prj.name .. "\"",
		"OYL_MODULE_CURRENT_AS_MACRO=" .. prj_name_as_macro,
	}

	-- SharedLib Export Macros
	usage "PUBLIC"; do
		defines {
			"OYL_MODULE_AVAILABLE_" .. prj_name_as_macro,
		}
		filter { "kind:SharedLib" }; do
			defines {
				prj_name_as_macro .. "_SHAREDLIB"
			}
		end
		filter { "kind:StaticLib" }; do
			defines {
				prj_name_as_macro .. "_STATICLIB",
				prj_name_as_macro .. "_EXPORT=", -- Already defined for SharedLib in other usages
			}
		end
	end
	usage "PRIVATE"; do
		filter { "kind:SharedLib" }; do
			defines {
				prj_name_as_macro .. "_EXPORT=__OYL__EXPORT_ATTR"
			}
		end
	end
	usage "INTERFACE"; do
		filter { "kind:SharedLib" }; do
			defines {
				prj_name_as_macro .. "_EXPORT=__OYL__IMPORT_ATTR"
			}
		end
	end
end

function m.onProjectAddReflectionBuildAction(prj)
	local project_action_call_array = oyl3d.elements.postAction.onWorkspacePreProcess()
	assert(type(project_action_call_array) == "table")

	if not prj.reflection then
		-- Check if we've already overridden projectAction. If not, then override it
		if not table.contains(project_action_call_array, private.deleteDotGeneratedFolder) then
			premake.override(oyl3d.elements.postAction, "onProjectPreProcess", function(base, prj)
				local result = base(prj)
				table.insert(result, private.deleteDotGeneratedFolder)
				return result
			end)
		end
		return
	end

	project(prj.name)

	dependson {
		"Oyl.Spyll",
	}

	-- Handle this logic in tokenized strings to ensure proper project filters are applied
	local spyllCommand = path.join(Config.BinariesDir, "Oyl.Spyll.exe")

	local function appendToCommand(str)
		-- Append trimmed string after space
		spyllCommand = spyllCommand .. " " .. str:match("^%s*(.-)%s*$")
	end

	-- Oyl.Spyll Commands
	appendToCommand [[%{table.concat(
		table.translate(
				table.filter(
					cfg.files,
					function(file) return path.hasextension(file, ".h") end
				),
				function(file) return '"' .. file .. '"' end
			),
			" "
		)}]]

	appendToCommand '"-assembly=%{cfg.buildtarget.basename}"'
	appendToCommand [[
		%{table.concat(
			table.translate(
				cfg.links,
				function(link) return '"-dependency=' .. link .. '"' end
			),
			" "
		)}
	]]

	-- CC1 commands
	appendToCommand '--'
	appendToCommand '-xc++'                         -- language
	appendToCommand '"-std=%{cfg.cppdialect:lower()}"' -- C++ Standard

	-- Include directories
	appendToCommand [[
		%{table.concat(
			table.translate(
				cfg.includedirs,
				function(dir) return '"-I' .. dir .. '"' end
			),
			" "
		)}
	]]

	-- External Include Directories
	appendToCommand [[
		%{table.concat(
			table.translate(
				cfg.externalincludedirs,
				function(dir) return '"-isystem' .. dir .. '"' end
			),
			" "
		)}
	]]
	if p.action.current() and p.action.current().vstudio then
		appendToCommand '"-isystem$(IncludePath)"'
	end

	-- Defines
	appendToCommand [[
		%{table.concat(
			table.translate(
				cfg.defines,
				function(define) return '-D' .. define end
			),
			" "
		)}
	]]

	-- Only include --pch arg if project has a pch
	-- Force include the pch file
	appendToCommand [[%{cfg.pchheader and '"-include' .. cfg.pchheader .. '"' or ''}]]

	prebuildmessage("Executing " .. spyllCommand)

	prebuildcommands {
		"cd %{prj.location}",
		spyllCommand
	}

	files {
		path.join("%{wks.basedir}", "GeneratedInclude.cpp")
	}

	-- Check if we've already overridden projectAction. If not, then override it
	if not table.contains(project_action_call_array, private.addDotGeneratedToPrjDotIncludeDir) then
		premake.override(oyl3d.elements.postAction, "onProjectPreProcess", function(base, prj)
			local result = base(prj)
			table.insert(result, private.addDotGeneratedToPrjDotIncludeDir)
			return result
		end)
	end

	-- Ensure files under .Generated aren't included in the build
	local generatedDir = ".Generated"
	local generatedFilesPattern = path.join(generatedDir, "**")
	if prj.reflectionshowgenerated then
		filter { "files:" .. generatedFilesPattern }; do
			excludefrombuild "On"
		end; filter {}
	else
		removefiles {
			generatedFilesPattern
		}
	end
end

function m.onProjectDefinePackageMacros(prj)
	local prj_name_as_macro = prj.name:upper():gsub("[%.%-% ]", "_")

	defines {
		"OYL_PACKAGE_WITHIN_" .. prj_name_as_macro,
		"OYL_PACKAGE_CURRENT=\"" .. prj.name .. "\"",
		"OYL_PACKAGE_CURRENT_AS_MACRO=" .. prj_name_as_macro,
	}

	-- SharedLib Export Macros
	usage "PUBLIC"; do
		defines {
			"OYL_PACKAGE_AVAILABLE_" .. prj_name_as_macro,
		}
		filter { "kind:SharedLib" }; do
			defines {
				prj_name_as_macro .. "_SHAREDLIB"
			}
		end
		filter { "kind:StaticLib" }; do
			defines {
				prj_name_as_macro .. "_STATICLIB",
			}
		end
	end
end

function m.onProjectRemoveLinksToNoneKindProjects(prj)
	local wks = prj.workspace
	for index = #prj.links, 1, -1 do
		local link = prj.links[index]
		local linkprj = wks.projects[link]
		if linkprj and linkprj.kind == "None" then
			removelinks {
				link
			}
		end
	end
end

function m.onProjectRemoveStaticLibLinks(prj)
	filter { "kind:StaticLib" }; do
		removelinks { prj.links }
	end; filter {}
end

function m.onWorkspaceRemoveNonProjectPackages(wks)
	if not p.action.isConfigurable() then
		return
	end

	-- Remove all references to non-project packages
	-- Do this before we remove the projects from the workspace so we can distiguish between a raw
	-- library links and package links
	for _, prj in ipairs(wks.projects) do
		for linkindex = #prj.links, 1, -1 do
			local link = prj.links[linkindex]
			local linkprj = wks.projects[link]
			if linkprj and linkprj._package and not linkprj._package.OnProject then
				project(prj.name)
				removelinks {
					link
				}
			end
		end
	end
	workspace(wks.name)

	-- Remove all non-project packages from the workspace
	-- We only want the link as an indicator to call the OnDepend function.
	-- If no project is generated, there is no library to depend on
	for prjindex = #wks.projects, 1, -1 do
		local prj = wks.projects[prjindex]
		if prj._package and not prj._package.OnProject then
			table.remove(wks.projects, prjindex)
		end
	end
end

function m.onWorkspaceRemoveUnreferencedProjects(wks)
	if not p.action.isConfigurable() then
		return
	end

	-- Gather the set of projects referencing or being referenced by another project
	local dependSet = {}
	for _, prj in pairs(wks.projects) do
		if #prj.links > 0 then
			dependSet[prj.name] = true
		end
		for _, link in ipairs(prj.links) do
			dependSet[link] = true
		end
		for _, depend in ipairs(prj.dependson) do
			dependSet[depend] = true
		end
		for _, uses in ipairs(prj.uses) do
			dependSet[uses] = true
		end
	end

	-- Remove all projects not referencing or being referenced by another project
	for index = #wks.projects, 1, -1 do
		local prj = wks.projects[index]
		if not dependSet[prj.name] then
			printf("Unreferenced package '%s' will not be Generated...", prj.name)
			table.remove(wks.projects, index)
		end
	end
end

return m
