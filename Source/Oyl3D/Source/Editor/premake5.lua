local Project = require "Project"
local Config = require "Config"

group "Oyl/Editor"

project "Oyl.Editor"; do
	language "C++"
	kind "SharedLib"
	
	Project.Files()
	
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
