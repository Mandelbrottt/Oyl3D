local Config = require "Config"

group "Editor"

project "Editor"; do
	language(premake.CPP)
	kind(premake.SHAREDLIB)
	
	pchheader "pch.h"
	pchsource "pch.cpp"

	removeconfigurations {
		Config.Configurations.Distribution
	}
	
	removeplatforms {
		Config.Platforms.Standalone
	}
	
	links {
		"Core",
		"ImGui",
		"SpdLog",
		"TracyClient",
	}
end
