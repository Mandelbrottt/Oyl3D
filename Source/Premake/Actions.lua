local Actions = {}

---@class SetActionOptionsParams
---@field Trigger string
---@field Keep? string[]
---@field Options? fun()

---@param params SetActionOptionsParams
function Actions.SetActionOptions(params)
	local trigger = params.Trigger

	-- If a the given trigger is not set, return early
	if not trigger or _ACTION ~= trigger then
		return
	end

	-- Generate a set from the given keep options
	local keepSet = {}
	table.foreachi(params.Keep, function(value) keepSet[value] = true end)

	-- If the given option's key was not specified, remove it
	local options = premake.option.list
	local optionKeys = table.keys(options)
	for _, key in ipairs(optionKeys) do
		if not keepSet[key] then
			options[key] = nil
		end
	end

	-- Only keep the trigger action in the action list
	local thisAction = premake.action.get(trigger)
	premake.action._list = { [trigger] = thisAction }

	-- Run callback for user specified options
	if params.Options then
		params.Options()
	end

	-- Prevent further creation of options and actions
	premake.override(premake.option, "add", function(base) end)
	premake.override(premake.action, "add", function(base) end)
end

return Actions
