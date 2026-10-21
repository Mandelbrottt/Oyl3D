local p = premake

local oyl3d = p.modules.oyl3d
oyl3d.postBake = oyl3d.postBake or {}

local m = oyl3d.postBake
local private = {}

function m.onConfigDefineModuleDependencies(cfg)
	local prj = cfg.project
	if prj._package then
		return
	end

	local dependenciesString = table.implode(
		table.filter(
			cfg.links,
			function(prj_link)
				local link_prj = prj.workspace.projects[prj_link]
				return link_prj and not link_prj._package
			end
		),
		'"',
		'"',
		','
	)
	if dependenciesString == "" then
		dependenciesString = '""'
	end
	table.insert(
		cfg.defines,
		prj.name:upper():gsub("[%.%-% ]", "_") .. '_DEPENDENCIES={ ' .. dependenciesString .. ' }'
	)
end

function m.onConfigConnectProjectLinks(cfg)
	local prj = cfg.project
	local wks = prj.workspace

	for link_index = #cfg.links, 1, -1 do
		local link = cfg.links[link_index]
		local link_prj = wks.projects[link]
		if not link_prj then
			goto continue
		end

		local link_cfg = p.project.getconfig(link_prj, cfg.buildcfg, cfg.platform)
		if not link_cfg then
			goto continue
		end

		if link_cfg.kind == "None" then
			cfg.links[link] = nil
			table.remove(cfg.links, link_index)
		end

		-- recurse links, add all children as links to parent projects for non-editor platform
		local function addLinksRecursive(link_cfg)
			for _, link in ipairs(link_cfg.links) do
				cfg.links[link] = link
				table.insert(cfg.links, link)
			end
			for _, link in ipairs(link_cfg.links) do
				local link_cfg_link_prj = wks.projects[link]
				local link_cfg_link_cfg = p.project.getconfig(link_cfg_link_prj, cfg.buildcfg, cfg.platform)
				if link_cfg_link_cfg then
					addLinksRecursive(link_cfg_link_cfg)
				end
			end
		end
		if cfg.kind == "StaticLib" and cfg.platform == "Standalone" then
			addLinksRecursive(link_cfg)
		end

		local package = link_prj._package

		::continue::
	end
end

return m
