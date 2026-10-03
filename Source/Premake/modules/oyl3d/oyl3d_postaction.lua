local p = premake

local oyl3d = p.modules.oyl3d
oyl3d.postAction = oyl3d.postAction or {}

local m = oyl3d.postAction
local private = {}

function m.onWorkspacePrepare(wks)
	local wksDotIncludeDir = oyl3d.wksDotIncludeDir(wks)

	if os.isdir(wksDotIncludeDir) then
		os.rmdir(wksDotIncludeDir)
	end
end

function m.onProjectCreateLinkDotIncludeDirs(prj)
	local wks = prj.workspace

	oyl3d.prjAddEntryToDotInclude(prj, prj.basedir)

	link_set = {}
	for _, link in ipairs(prj.links) do
		link_set[link] = true
	end
	for _, use in ipairs(prj.uses) do
		link_set[use] = true
	end

	for link, _ in pairs(link_set) do
		local link_prj = wks.projects[link]
		if not link_prj then
			goto continue
		end
		-- Non-source project, disregard
		if link_prj.basedir == wks.basedir then
			goto continue
		end

		local link_dirs = link_prj.packageincludedirs

		if not link_dirs then
			-- if unset, use basedir
			link_dirs = link_prj.basedir
		end

		-- Add symlink from link dir to project .Include folder
		if type(link_dirs) ~= "table" then
			local link_dir = link_dirs
			local link_name = path.getname(link_dir)
			if link_prj._package and link_dir == link_prj.basedir then
				link_name = link_prj.name
			end
			oyl3d.prjAddEntryToDotInclude(prj, link_dir, link_name)
		else
			for _, link_dir in ipairs(link_dirs) do
				oyl3d.prjAddEntryToDotInclude(prj, link_dir)
			end
		end

		::continue::
	end
end

return m