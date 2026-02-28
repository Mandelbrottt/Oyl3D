require("Oyl")

---@class Library
---@field Name? string
---@field IncludeDirs string[]
---@field LibraryDirs string[]
---@field Libraries string[]
---@field DependentProperties? fun(library: Library)

---@type { [string]: Library }
Oyl.Libraries = {}

Oyl.Library = {}
local Library = Oyl.Library

function Oyl.Library.SetupLibraries()
	for name, library in pairs(Oyl.Libraries) do
		library.Name = name
	end
end

return Library