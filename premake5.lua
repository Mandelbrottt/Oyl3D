workspace "Yr2Engine"
	architecture "x64"

	configurations {
		"Debug",
		"Release",
		"Dist"
	}	
	
	startproject "Yr2Game"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["glfw"] = "Engine/vendor/glfw/include/"
IncludeDir["glad"] = "Engine/vendor/glad/include/"
IncludeDir["imgui"] = "Engine/vendor/imgui/"
IncludeDir["glm"] = "Engine/vendor/glm/"

include "Engine/vendor/glad/"
include "Engine/vendor/glfw/"
include "Engine/vendor/imgui/"

project "Yr2Engine"
	location "Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "y2pch.h"
	pchsource "Engine/src/y2pch.cpp"

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	defines {
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs {
		"%{prj.name}/src/",
		"%{prj.name}/vendor/spdlog/include/",
		"%{IncludeDir.glfw}",
		"%{IncludeDir.glad}",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.glm}"
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
			"Y2_PLATFORM_WINDOWS",
			"Y2_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

		-- postbuildcommands {
		-- 	("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"")
		-- }

	filter "configurations:Debug"
		defines "Y2_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "Y2_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "Y2_DIST"
		runtime "Release"
		optimize "on"

project "Yr2Game"
	location "Game"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs {
		"Hazel/vendor/spdlog/include",
		"Hazel/src/",
		"Hazel/vendor/",
		"%{IncludeDir.glm}"
	}

	links {
		"Yr2Engine"
	}

	filter "system:windows"
		systemversion "latest"

		defines {
			"Y2_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "Y2_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "Y2_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "Y2_DIST"
		runtime "Release"
		optimize "on"
