local Config = require "Config"

---@type { [string]: { OnProject: fun(prj), OnDepend: fun(prjcfg, packagecfg)? } }
local Packages = {
	Clang = {
		OnProject = function(prj)
			language "C++"
			kind "None"

			packageincludedirs {
				"include/clang",
				"include/clang-c",
				"include/llvm",
				"include/llvm-c",
			}

			filter "action:vs*"; do
				files {
					"**.natvis"
				}
			end

			usage "INTERFACE"; do
				links {
					os.matchfiles(path.join("lib", "*.lib")),
					"ntdll",
					"version"
				}
				runtime "Release"
				defines {
					"_ITERATOR_DEBUG_LEVEL=0"
				}
			end
		end
	},
	Glfw = {
		OnProject = function(prj)
			language "C"
			kind "SharedLib"

			files {
				"include/GLFW/**",
				"src/**",
			}

			includedirs {
				"include/GLFW"
			}

			packageincludedirs "include/GLFW"

			defines { "_CRT_SCURE_NO_WARNINGS" }

			filter "system:windows"; do
				defines { "_GLFW_WIN32" }
			end
			filter "kind:StaticLib"; do
				defines {}
			end
			filter "kind:SharedLib"; do
				defines { "_GLFW_BUILD_DLL" }
			end

			usage "INTERFACE"; do
				links { prj.name }
				filter { "kind:SharedLib" }; do
					defines { "GLFW_DLL" }
				end
			end
		end,
	},
	ImGui = {
		OnProject = function(prj)
			language "C++"
			kind "StaticLib" -- ImGui SharedLib support is quite involved, so don't bother for now

			files {
				"imconfig.h",
				"imgui*.h",
				"imgui*.cpp",
				"imstb*.h",
			}

			includedirs { "." }

			packageincludedirs "."

			filter "action:vs*"; do
				files {
					"**.natvis"
				}
			end

			usage "INTERFACE"; do
				links { prj.name }
			end
		end
	},
	NlohmannJson = {
		OnProject = function(prj)
			language "C++"
			kind "None"

			files {
				"include/**",
			}

			includedirs {
				"include/nlohmann"
			}

			packageincludedirs "include/nlohmann"

			filter "action:vs*"; do
				files {
					"**.natvis"
				}
			end
		end
	},
	SpdLog = {
		OnProject = function(prj)
			language "C++"
			kind "StaticLib"

			files {
				"src/**",
				"include/**"
			}

			includedirs {
				"include"
			}

			packageincludedirs "include/spdlog"

			defines {
				"SPDLOG_LEVEL_NAMES={" ..
				'	spdlog::string_view_t("TRACE", 5),' ..
				'	spdlog::string_view_t("DEBUG", 5),' ..
				'	spdlog::string_view_t("INFO", 4),' ..
				'	spdlog::string_view_t("WARNING", 7),' ..
				'	spdlog::string_view_t("ERROR", 5),' ..
				'	spdlog::string_view_t("FATAL", 5),' ..
				'	spdlog::string_view_t("OFF", 3),' ..
				"}",
				"SPDLOG_SHORT_LEVEL_NAMES={" ..
				'	"T", "D", "I", "W", "E", "F", "O"' ..
				"}",
			}
			filter { "kind:SharedLib" }; do
				defines {
					"spdlog_EXPORTS",
					"FMT_LIB_EXPORT",
				}
			end

			usage "PUBLIC"; do
				defines {
					"SPDLOG_COMPILED_LIB",
					"_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING",
					"FMT_UNICODE=0",
					"FMT_USE_CONSTEVAL=0",
				}
				filter { "kind:SharedLib" }; do
					defines {
						"SPDLOG_SHARED_LIB",
						"FMT_SHARED",
					}
				end
			end
			usage "INTERFACE"; do
				links { prj.name }
			end
		end,
	},
	TracyClient = {
		OnProject = function(prj)
			language "C++"
			kind "SharedLib"

			removeconfigurations {
				Config.Configurations.Debug,
				Config.Configurations.Development,
				Config.Configurations.Distribution
			}

			files {
				"public/TracyClient.cpp",
				"public/tracy/**.h",
				"public/tracy/**.hpp",
				"public/common/**.h",
				"public/common/**.hpp",
			}

			includedirs {
				"public"
			}

			defines {
				"TRACY_NO_SAMPLING",
				"TRACY_ONLY_LOCALHOST", -- TODO: Fix only localhost at runtime
			}
			filter "kind:SharedLib"; do
				defines {
					"TRACY_EXPORTS"
				}
			end
			filter { "configurations:not " .. Config.Configurations.Profile }; do
				excludefrombuild "On"
			end

			usage "PUBLIC"; do
				defines {
					"TRACY_ENABLE",
					"TRACY_DELAYED_INIT",
					"TRACY_MANUAL_LIFETIME",
					"TRACY_NO_SYSTEM_TRACING",
				}
			end
			usage "INTERFACE"; do
				defines {
					"TRACY_IMPORTS",
				}

				links { prj.name }
			end
		end,
	},
	Vulkan = {
		OnProject = function(prj)
			language "C++"
			kind "Makefile"

			files {
				"Include/**.cpp",
				"Include/**.h",
				"Include/**.hpp",
			}

			includedirs {
				"Include"
			}

			packageincludedirs {
				"Include/vulkan",
				"Include/vk_video",
				"Include/dxc",
			}

			libdirs {
				"Lib"
			}

			filter { "configurations:*" .. Config.Configurations.Distribution .. "*" }; do
				local sharedLibsToCopy = {
					"dxcompiler.dll"
				}
				for _, sharedLib in ipairs(sharedLibsToCopy) do
					local inFile = path.join(prj.basedir, path.join("Bin", sharedLib))
					local outFile = path.join(Config.BinariesDir, sharedLib)
					local surround = function(str) return "%[" .. str .. "]" end

					local buildcommand = string.format("{COPYFILE} %s %s", surround(inFile), surround(outFile))
					buildcommands { buildcommand }
					rebuildcommands { buildcommand }
					cleancommands { "{DELETE} " .. surround(outFile) }
				end
			end
			filter { "configurations:not *" .. Config.Configurations.Distribution .. "*" }; do
				excludefrombuild "On"
			end
			filter {}

			usage "INTERFACE"; do
				defines {
					"VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1",
					"VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1",
					"VULKAN_HPP_HANDLE_ERROR_OUT_OF_DATE_AS_SUCCESS=1"
				}
				links {
					"vulkan-1",
					"dxcompiler"
				}
			end
		end,
	},
	YamlCpp = {
		OnProject = function(prj)
			language "C++"
			kind "SharedLib"

			files {
				"src/**",
				"include/**",
			}

			includedirs {
				"include"
			}

			packageincludedirs "include/yaml-cpp"

			filter "kind:SharedLib"; do
				defines { "yaml_cpp_EXPORTS" }
			end

			usage "PUBLIC"; do
				links { prj.name }
				filter "kind:StaticLib"; do
					defines { "YAML_CPP_STATIC_DEFINE" }
				end
			end
		end
	},
}

return Packages
