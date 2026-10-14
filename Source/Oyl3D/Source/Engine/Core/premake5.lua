local oyl3d = premake.modules.oyl3d

group "Oyl/Engine"

project "Core"; do
	language "C++"
	kind "SharedLib"

	oyl3d.project.files()

	-- reflection "On"

	uses { "Tracy" } -- TEMP: Fix uses in usage block not being picked up by oyl project gen
	usage "PUBLIC"; do
		uses {
			"Tracy"
		}
	end
	usage "PRIVATE"; do
		forceincludes {
			"Core/Preprocessor/Common.h"
		}
	end
	usage "INTERFACE"; do
		-- Premake adds forceincludes as relative paths no matter what
		-- drop the "Core" for other projects to not use the .Include version
		forceincludes {
			"Preprocessor/Common.h"
		}
	end
end
