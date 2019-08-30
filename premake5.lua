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
		"OYL_ENGINE"
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
		"%{IncludeDir.stb}"
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
		defines { "OYL_DEBUG", "OYL_LOG_CONSOLE" }
		runtime "Debug"
		symbols "on"

	filter "configurations:Development"
		defines { "OYL_DEVELOPMENT", "OYL_LOG_CONSOLE" }
		runtime "Release"
		optimize "on"

	filter "configurations:Distribution"
		defines "OYL_DISTRIBUTION"
		runtime "Release"
		optimize "on"

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

	defines {
		"OYL_GAME"
	}

	postbuildcommands {
		"{COPY} %{prj.location}res/ %{wks.location}bin/"..outputdir.."/Game/res/",
		"{COPY} %{prj.location}imgui.ini* %{wks.location}bin/"..outputdir.."/Game/imgui.ini*"
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

	filter "system:windows"
		entrypoint "wWinMainCRTStartup"
		systemversion "latest"

		defines {
			"OYL_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines { "OYL_DEBUG", "OYL_LOG_CONSOLE" }
		runtime "Debug"
		symbols "On"

	filter "configurations:Development"
		defines { "OYL_DEVELOPMENT", "OYL_LOG_CONSOLE" }
		runtime "Release"
		optimize "on"

	filter "configurations:Distribution"
		defines "OYL_DISTRIBUTION"
		runtime "Release"
		optimize "on"
