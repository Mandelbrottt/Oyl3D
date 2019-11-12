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
IncludeDir["spdlog"] = "Engine/vendor/spdlog/include/"
IncludeDir["glfw"] = "Engine/vendor/glfw/include/"
IncludeDir["glad"] = "Engine/vendor/glad/include/"
IncludeDir["imgui"] = "Engine/vendor/imgui/"
IncludeDir["glm"] = "Engine/vendor/glm/"
IncludeDir["stb"] = "Engine/vendor/stb/"
IncludeDir["entt"] = "Engine/vendor/entt/src/"
IncludeDir["fmod"] = "Engine/vendor/fmod/include/"
IncludeDir["gainput"] = "Engine/vendor/gainput/lib/include/"
IncludeDir["entt"] = "Engine/vendor/entt/src/"
IncludeDir["json"] = "Engine/vendor/nlohmann/single_include/"
IncludeDir["bullet"] = "Engine/vendor/bullet3/src/"

group "Dependencies"
	include "Engine/vendor/glad/"
	include "Engine/vendor/glfw/"
	include "Engine/vendor/imgui/"
	include "Engine/vendor/gainput/"

	group "Dependencies/Bullet3"
		include "Engine/vendor/bullet3/src/BulletDynamics"
		include "Engine/vendor/bullet3/src/BulletCollision"
		include "Engine/vendor/bullet3/src/LinearMath"

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
		"Engine/src/**.cpp",
		"Engine/src/**.inl"
	}

	defines {
		"OYL_ENGINE",
		"_CRT_SECURE_NO_WARNINGS"
	}

	postbuildcommands {
		"{COPY} %{prj.location}res/ %{wks.location}bin/" .. outputdir .. "/Engine/res/"
	}

	includedirs {
		"Engine/src/",
		"Engine/src/Oyl3D/",
		"Engine/vendor/",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.glfw}",
		"%{IncludeDir.glad}",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.fmod}",
		"%{IncludeDir.gainput}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.json}",
		"%{IncludeDir.bullet}"
	}

	libdirs {
		"Engine/vendor/fmod/lib/"
	}

	links {
		"GLFW",
		"Glad",
		"ImGui",
		"Gainput",
		"BulletDynamics",
		"BulletCollision",
		"LinearMath",
		"opengl32"
	}

	filter "system:windows"
		systemversion "latest"

		defines {
			"GLFW_INCLUDE_NONE", 
			"WIN32_LEAN_AND_MEAN"
		}
		
		prebuildcommands {
			"%{wks.location}scripts/windows/GenerateVSProjects.bat"
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
			"fmodstudio_vc.lib"
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
		"Game/src/**.cpp",
		"Game/src/**.inl"
	}

	excludes { "**/ClientAppTemplate.cpp" }

	defines {
		"OYL_GAME",
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs {
		"Engine/src/",
		"Engine/vendor/",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb}",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.json}",
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
