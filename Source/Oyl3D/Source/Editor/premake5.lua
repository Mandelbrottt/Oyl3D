local oyl3d = premake.modules.oyl3d

local Config = require "Config"

group "Oyl/Editor"

project "Oyl.Editor"; do
	language "C++"
	kind "SharedLib"
	
	oyl3d.project.files()
	
	removeconfigurations {
		Config.Configurations.Distribution
	}
	
	removeplatforms {
		Config.Platforms.Standalone
	}

	reflection "On"
	
	links {
		"Oyl.Core",
	}
	
	links {
		"ImGui",
		"SpdLog",
		"TracyClient",
	}
end
