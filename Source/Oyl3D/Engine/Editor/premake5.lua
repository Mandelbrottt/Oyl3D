local Oyl3D = require "Oyl3D"
local Config = require "Config"

group "Oyl/Editor"

Oyl3D.CppProject "Oyl.Editor"; do
	kind "SharedLib"
	
	removeconfigurations {
		Config.Configurations.Distribution
	}
	
	removeplatforms {
		Config.Platforms.Standalone
	}

	reflection "On"
	
	links {
		"Oyl.Core",
		"ImGui",
		"SpdLog",
		"TracyClient",
	}
end
