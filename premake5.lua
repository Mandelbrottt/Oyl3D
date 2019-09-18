workspace "Oyl3D"
	architecture "x64"

	configurations {
		"Debug",
		"Development",
		"Distribution"
	}	
	
	startproject "OylGame"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["glfw"] = "Engine/vendor/glfw/include/"
IncludeDir["glad"] = "Engine/vendor/glad/include/"
IncludeDir["imgui"] = "Engine/vendor/imgui/"
IncludeDir["glm"] = "Engine/vendor/glm/"
IncludeDir["stb"] = "Engine/vendor/stb/"
IncludeDir["entt"] = "Engine/vendor/entt/src/"
IncludeDir["fmod"] = "Engine/vendor/fmod/include/"

group "Dependencies"
	include "Engine/vendor/glad/"
	include "Engine/vendor/glfw/"
	include "Engine/vendor/imgui/"
group ""

project "OylEngine"
	location "Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/Engine")
	objdir ("bin-int/" .. outputdir .. "/Engine")

	pchheader "oylpch.h"
	pchsource "Engine/src/Oyl3D/oylpch.cpp"

	files {
		"Engine/src/**.h",
		"Engine/src/**.cpp"
	}

	defines {
		"OYL_ENGINE",
		"_CRT_SECURE_NO_WARNINGS"
	}

	postbuildcommands {
		"{COPY} %{prj.location}res/ %{wks.location}bin/"..outputdir.."/Engine/res/"
	}

	includedirs {
		"Engine/src/",
		"Engine/src/Oyl3D/",
		"Engine/vendor/spdlog/include/",
		"%{IncludeDir.glfw}",
		"%{IncludeDir.glad}",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb}",
		"%{IncludeDir.entt}"
		"%{IncludeDir.fmod}"
	}

	libdirs {
		"Engine/vendor/fmod/lib/"
	}

	links {
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines {
			"OYL_PLATFORM_WINDOWS",
			"GLFW_INCLUDE_NONE", 
			"WIN32_LEAN_AND_MEAN"
		}

	filter "configurations:Debug"
		defines { "OYL_DEBUG" }
		runtime "Debug"
		symbols "on"
		links {
			"fmodL_vc.lib",
			"fmodstudioL_vc.lib"
		}

	filter "configurations:Development"
		defines { "OYL_DEVELOPMENT" }
		runtime "Release"
		optimize "on"

	filter "configurations:Distribution"
		defines { "OYL_DISTRIBUTION" }
		runtime "Release"
		optimize "on"

	filter "configurations:not Debug"
		links {
			"fmod_vc.lib",
			"fmod_studio_vc.lib"
		}

	filter "configurations:not Development"

	filter "configurations:not Distribution"
		defines { "OYL_LOG_CONSOLE" }

project "OylGame"
	location "Game"
	kind "WindowedApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/Game")
	objdir ("bin-int/" .. outputdir .. "/Game")

	files {
		"Game/src/**.h",
		"Game/src/**.cpp"
	}

	excludes { "**/ClientAppTemplate.cpp" }

	defines {
		"OYL_GAME",
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs {
		"Engine/vendor/spdlog/include",
		"Engine/src/",
		"Engine/vendor/",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb}",
		"%{IncludeDir.imgui}"
	}

	links {
		"OylEngine"
	}

	postbuildcommands {
		"{COPY} %{prj.location}res/ %{wks.location}bin/"..outputdir.."/Game/res/"
	}
	
	filter "system:windows"
		entrypoint "wWinMainCRTStartup"
		systemversion "latest"
	
		defines {
			"OYL_PLATFORM_WINDOWS",
			"WIN32_LEAN_AND_MEAN",
			"NOMINMAX"
		}		
	
	filter "configurations:Debug"
		defines { "OYL_DEBUG" }
		runtime "Debug"
		symbols "On"
	
	filter "configurations:Development"
		defines { "OYL_DEVELOPMENT" }
		runtime "Release"
		optimize "on"
	
	filter "configurations:Distribution"
		defines "OYL_DISTRIBUTION"
		runtime "Release"
		optimize "on"

	filter "configurations:not Distribution"
		defines { "OYL_LOG_CONSOLE" }
		postbuildcommands {
			"{COPY} %{prj.location}imgui.ini* %{wks.location}bin/"..outputdir.."/Game/imgui.ini*"
		}
