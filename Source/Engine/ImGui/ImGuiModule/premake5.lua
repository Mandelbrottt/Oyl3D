local oyl3d = premake.modules.oyl3d

group "Oyl/Engine"

project "ImGuiModule"; do
	language "C++"
	kind "SharedLib"

	oyl3d.project.files()

	-- reflection "On"

	-- ImGui_Impl* functions use #include "imgui.h"
	includedirsafter {
		'%{wks.projects["ImGui"].basedir}'
	}

	usage "PUBLIC"; do
		uses {
			"Core",
			"Application",
			"Rendering",
			"ImGui",
		}
	end
end
