-- local Config = require "Config"
-- local Check = require "CheckProject"
-- local Package = require "Package"

-- workspace "Spyll"; do
-- 	location "./"
-- 	filename("%{wks.name}_" .. _ACTION)

-- 	configurations {
-- 		"Debug",
-- 		"RelWithDebInfo",
-- 		"Release"
-- 	}

-- 	platforms {
-- 		premake.X86_64
-- 	}

-- 	project "Oyl.Spyll"; do
-- 		location "Spyll"
-- 		kind(premake.CONSOLEAPP)
		
-- 		includedirs {
-- 			"%{prj.location}",
-- 		}
		
-- 		filename("%{prj.name}_" .. _ACTION)
-- 		language "C++"
-- 		cppdialect "C++17"
-- 		staticruntime "Off"
-- 		warnings "Extra"
-- 		fatalwarnings { "All" }
-- 		multiprocessorcompile "On"
-- 		externalwarnings "Off"
-- 		externalanglebrackets "On"
-- 		runtime "Release" -- ClangTooling compiled with release runtime, needs to match

-- 		targetdir(Config.TargetDir .. Config.OutputDir)
-- 		debugdir(Config.TargetDir .. Config.OutputDir)
-- 		objdir(Config.ObjectDir .. Config.OutputDir)
-- 		implibdir(Config.LibraryDir .. Config.OutputDir)

-- 		Package.Include(Packages.ClangTooling)

-- 		files {
-- 			"./Spyll/**.cpp",
-- 			"./Spyll/**.h",
-- 			"./Spyll/**.hpp",
-- 			"./Spyll/**.inl",
-- 			"./Spyll/**.ixx",
-- 		}

-- 		files {
-- 			"./Tests/**.cpp",
-- 			"./Tests/**.h",
-- 			"./Tests/**.hpp",
-- 		}

-- 		filter "files:*Tests/**.cpp"; do
-- 			excludefrombuild "On"
-- 		end
		
-- 		defines {
-- 			"ITERATOR_DEBUG_LEVEL=0"
-- 		}

-- 		filter "system:windows"; do
-- 			links {
-- 				"ntdll",
-- 				"version",
-- 				-- "psapi",
-- 				-- "ws2_32",
-- 				-- "-delayload:shell32.dll",
-- 				-- "-delayload:ole32.dll",
-- 			}
-- 		end
		
-- 		filter "configurations:Debug"; do
-- 			optimize "Off"
-- 			symbols "On"
-- 		end

-- 		filter "configurations:RelWithDebInfo"; do
-- 			optimize "Debug"
-- 			symbols "On"
-- 		end

-- 		filter "configurations:Release"; do
-- 			optimize "Full"
-- 			symbols "Off"
-- 		end
-- 	end
-- end
