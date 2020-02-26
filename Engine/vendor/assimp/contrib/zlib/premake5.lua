project "zlib"
	kind "StaticLib"
	language "C"
	staticruntime "on"
	
	targetdir ("bin/")
	objdir ("bin-int/")


	files {
		"*.c",
		"*.h",
	}
	
	filter "system:windows"
		systemversion "latest"
		
		defines { 
			"_CRT_SECURE_NO_WARNINGS"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"
		targetsuffix "L"

	filter "configurations:not Debug"
		runtime "Release"
		optimize "on"
