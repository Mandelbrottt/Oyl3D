local Config = require "Config"

---@type { [string]: { OnProject: fun(prj), OnDepend: fun(prjcfg, packagecfg)? } }
local Packages = {
	Clang = {
		Remote = {
			Url = (function()
				if (os.host() == premake.WINDOWS) then
					return "https://dl.dropbox.com/scl/fi/gaeyy54dlccesq7yuzvif/llvm.txz?rlkey=hxbbr6nv963xgclo36bokcr3o"
				end
				return ""
			end)()
		},
		OnProject = function(prj)
			language "C++"
			kind "None"

			filter "action:vs*"; do
				files {
					"**.natvis"
				}
			end

			usage "INTERFACE"; do
				includedirsafter {
					"include"
				}
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
	CppTrace = {
		Git = {
			Url = "https://github.com/jeremy-rifkin/cpptrace.git",
			Ref = "v1.0.4",
		},
	},
	Glfw = {
		Git = {
			Url = "https://github.com/glfw/glfw.git",
			Ref = "3.4",
		},
		OnProject = function(prj)
			language "C"
			kind "SharedLib"

			files {
				"include/GLFW/**",
				"src/**",
			}

			includedirs {
				"include/GLFW",
			}

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
				includedirsafter {
					"include",
				}
				filter { "kind:SharedLib" }; do
					defines {
						"GLFW_DLL",
					}
				end
			end
		end,
	},
	GoogleTest = {
		Git = {
			Url = "https://github.com/google/googletest.git",
			Ref = "v1.17.0",
		}
	},
	ImGui = {
		Git = {
			Url = "https://github.com/ocornut/imgui.git",
			Ref = "v1.92.8",
		},
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

			filter "action:vs*"; do
				files {
					"**.natvis",
				}
			end

			usage "INTERFACE"; do
				links { prj.name }
			end
		end
	},
	NlohmannJson = {
		Git = {
			Url = "https://github.com/nlohmann/json.git",
			Ref = "v3.11.3",
			Sparse = {
				"/*",
				"!/test",
				"!/benchmarks",
				"!/single_include",
			}
		},
		OnProject = function(prj)
			language "C++"
			kind "None"

			files {
				"include/**",
			}

			includedirs {
				"include/nlohmann"
			}

			filter "action:vs*"; do
				files {
					"**.natvis"
				}
			end

			usage "INTERFACE"; do
				includedirsafter {
					"include",
				}
			end
		end
	},
	SpdLog = {
		Git = {
			Url = "https://github.com/gabime/spdlog.git",
			Ref = "v1.17.0",
		},
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
				includedirsafter {
					"include"
				}
			end
		end,
	},
	Tracy = {
		Git = {
			Url = "https://github.com/wolfpld/tracy.git",
			Ref = "v0.13.1",
		},
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
				filter { "configurations:" .. Config.Configurations.Profile }; do
					defines {
						"TRACY_ENABLE",
						"TRACY_DELAYED_INIT",
						"TRACY_MANUAL_LIFETIME",
						"TRACY_NO_SYSTEM_TRACING",
						"TRACY_ON_DEMAND",
					}
				end
			end
			usage "INTERFACE"; do
				includedirsafter {
					path.getdirectory(prj.basedir)
				}
				filter { "kind:SharedLib" }; do
					defines {
						"TRACY_IMPORTS",
					}
				end
				filter { "configurations:" .. Config.Configurations.Profile }; do
					links { prj.name }
				end
			end
		end,
	},
	Vulkan = {
		Local = {
			Path = assert(
				os.getenv("VULKAN_SDK"),
				"Missing Vulkan SDK! Please install the Vulkan SDK (https://vulkan.lunarg.com/sdk/home)!"
			)
		},
		OnProject = function(prj)
			language "C++"
			kind "Makefile"

			files {
				"Include/**.cpp",
				"Include/**.h",
				"Include/**.hpp",
			}

			includedirs {
				"Include",
			}

			libdirs {
				"Lib",
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
				includedirsafter {
					"Include",
				}
				defines {
					"VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1",
					"VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1",
					"VULKAN_HPP_HANDLE_ERROR_OUT_OF_DATE_AS_SUCCESS=1"
				}
				libdirs {
					"Lib",
				}
				links {
					"vulkan-1",
				}
			end
		end,
	},
	YamlCpp = {
		Git = {
			Url = "https://github.com/jbeder/yaml-cpp.git",
			Ref = "yaml-cpp-0.9.0",
		},
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

			filter "kind:SharedLib"; do
				defines { "yaml_cpp_EXPORTS" }
			end
			usage "PUBLIC"; do
				filter "kind:StaticLib"; do
					defines { "YAML_CPP_STATIC_DEFINE" }
				end
			end
			usage "INTERFACE"; do
				links { prj.name }
				includedirsafter {
					"include"
				}
			end
		end
	},
}

return Packages
