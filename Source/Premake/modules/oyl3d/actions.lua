local p = premake

local oyl3d = premake.modules.oyl3d
oyl3d.actions = oyl3d.actions or {}

local m = oyl3d.actions
local private = {}

m.elements = {}

m.elements.keepOptions = function(add_func)
	return {
		m.elements.defaultOptions
	}
end

m.elements.defaultOptions = function(add_func)
	add_func {
		"help",
		"debugger",
		"verbose",
		"file",
		"scripts",
		"interactive"
	}
end

function m.newaction(action)
	-- premake's newaction
	newaction(action)

	private.bindAction(action)
end

function private.bindAction(action)
	if action ~= p.action.current() then
		return
	end

	p.override(p.main, "processCommandLine", function(base)
		if action == p.action.current() then
			private.isolateAction(action)
		end

		base()
	end)
end

function private.isolateAction(action)
	-- Use callArray to allow users to override
	local keep_options = {}
	p.callArray(m.elements.keepOptions, function(opts)
		assert(type(opts) == "table")
		table.move(opts, 1, #opts, #keep_options + 1, keep_options)
	end)

	-- Generate a set of options to keep
	local keep_set = {}
	table.foreachi(keep_options, function(value) keep_set[value] = true end)

	-- If the given option's key was not specified, remove it
	local options = p.option.list
	local optionKeys = table.keys(options)
	for _, key in ipairs(optionKeys) do
		if not keep_set[key] then
			options[key] = nil
		end
	end

	-- Call user supplied options function
	if type(action.options) == "function" then
		-- Cache and override premake.option.add for the call to action.options()
		local p_option_add = p.option.add
		p.override(p.option, "add", function(base, opt)
			if not opt.category then
				opt.category = action.trigger
			end
			base(opt)
		end)

		action.options()

		p.option.add = p_option_add
	end

	-- Keep only the trigger action in the action list
	p.action._list = { [action.trigger] = action }
end

return m
