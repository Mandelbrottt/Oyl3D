local Config = require "Config"

---@type { [string]: { OnProject: fun(prj), OnDepend: fun(prjcfg, packagecfg)? } }
local Packages = {
	Assimp = {
		Git = {
			Url = "https://github.com/assimp/assimp.git",
			Ref = "v6.0.5"
		},
		OnProject = function(prj)
			language "C++"
			kind "SharedLib"

			do
				local config_h = io.readfile("include/assimp/config.h.in")
				local config_h_lines = string.explode(config_h, "\n")
				config_h = ""
				for _, line in next, config_h_lines do
					if string.contains(line, "#cmakedefine") then
						line = "// " .. line
					end
					config_h = config_h .. line .. "\n"
				end
				io.writefile("include/assimp/config.h", config_h)
			end
			do
				local revision_h = io.readfile("include/assimp/revision.h.in")

				local current_sha = os.outputof("git rev-parse --short HEAD")
				revision_h = revision_h:gsub("@GIT_COMMIT_HASH@", current_sha)

				local current_branch = os.outputof("git rev-parse --abbrev-ref HEAD")
				revision_h = revision_h:gsub("@GIT_BRANCH@", current_branch)

				-- Update when Git.Ref is updated
				revision_h = revision_h:gsub("@ASSIMP_VERSION_MAJOR@", "6")
				revision_h = revision_h:gsub("@ASSIMP_VERSION_MINOR@", "0")
				revision_h = revision_h:gsub("@ASSIMP_VERSION_PATCH@", "5")
				revision_h = revision_h:gsub("@ASSIMP_PACKAGE_VERSION@", "0")

				revision_h = revision_h:gsub("@CMAKE_SHARED_LIBRARY_PREFIX@", "")
				revision_h = revision_h:gsub("@LIBRARY_SUFFIX@", "")
				revision_h = revision_h:gsub("@CMAKE_DEBUG_POSTFIX@", "")

				io.writefile("include/assimp/revision.h", revision_h)
			end
			do
				local zconf_h = io.readfile("contrib/zlib/zconf.h.in")
				io.writefile("contrib/zlib/zconf.h", zconf_h)
			end

			files {
				"include/**",

				"code/AssetLib/Collada/**",
				"code/AssetLib/FBX/**",
				"code/AssetLib/Obj/**",
				"code/AssetLib/glTFCommon/**",
				"code/AssetLib/glTF/**",
				"code/AssetLib/glTF2/**",

				"code/CApi/**",
				"code/Common/**",
				"code/Geometry/**",
				"code/Material/**",
				"code/Pbrt/**",
				"code/PostProcessing/**",

				"contrib/zlib/*",
				"contrib/unzip/**",
				"contrib/pugixml/**"
			}

			includedirs {
				".",
				"code",
				"include",

			}

			includedirsafter {
				"contrib",
				"contrib/pugixml/src",
				"contrib/rapidjson/include",
				"contrib/unzip",
				"contrib/utf8cpp/source",
				"contrib/zlib",
			}

			-- links {
			-- 	"zlib"
			-- }

			local excluded_file_formats = {
				"3D",
				"3DS",
				"3MF",
				"AC",
				"AMF",
				"ASE",
				"ASSBIN",
				"ASSJSON",
				"ASSXML",
				"B3D",
				"BLEND",
				"BVH",
				"C4D",
				"COB",
				"CSM",
				"DXF",
				"HMP",
				"IFC",
				"IQM",
				"IRR",
				"IRRMESH",
				"LWO",
				"LWS",
				"M3D",
				"MD2",
				"MD3",
				"MD5",
				"MDC",
				"MDL",
				"MMD",
				"MS3D",
				"NDO",
				"NFF",
				"OFF",
				"OGRE",
				"OPENGEX",
				"PLY",
				"Q3BSP",
				"Q3D",
				"RAW",
				"SIB",
				"SMD",
				"STEP",
				"STL",
				"TERRAGEN",
				"USD",
				"X",
				"X3D",
				"XGL",
			}

			for _, format in ipairs(excluded_file_formats) do
				defines {
					"ASSIMP_BUILD_NO_" .. format .. "_IMPORTER",
					"ASSIMP_BUILD_NO_" .. format .. "_EXPORTER",
				}
			end

			defines {
				-- "SWIG",
				-- "ASSIMP_BUILD_NO_OWN_ZLIB",

				-- "ASSIMP_BUILD_NO_MAKELEFTHANDED_PROCESS",
				-- "ASSIMP_BUILD_NO_FLIPUVS_PROCESS",
				-- "ASSIMP_BUILD_NO_FLIPWINDINGORDER_PROCESS",
				-- "ASSIMP_BUILD_NO_CALCTANGENTS_PROCESS",
				-- "ASSIMP_BUILD_NO_JOINVERTICES_PROCESS",
				-- "ASSIMP_BUILD_NO_TRIANGULATE_PROCESS",
				"ASSIMP_BUILD_NO_GENFACENORMALS_PROCESS",
				-- "ASSIMP_BUILD_NO_GENVERTEXNORMALS_PROCESS",
				"ASSIMP_BUILD_NO_REMOVEVC_PROCESS",
				"ASSIMP_BUILD_NO_SPLITLARGEMESHES_PROCESS",
				"ASSIMP_BUILD_NO_PRETRANSFORMVERTICES_PROCESS",
				"ASSIMP_BUILD_NO_LIMITBONEWEIGHTS_PROCESS",
				-- "ASSIMP_BUILD_NO_VALIDATEDS_PROCESS",
				"ASSIMP_BUILD_NO_IMPROVECACHELOCALITY_PROCESS",
				"ASSIMP_BUILD_NO_FIXINFACINGNORMALS_PROCESS",
				"ASSIMP_BUILD_NO_REMOVE_REDUNDANTMATERIALS_PROCESS",
				"ASSIMP_BUILD_NO_FINDINVALIDDATA_PROCESS",
				"ASSIMP_BUILD_NO_FINDDEGENERATES_PROCESS",
				"ASSIMP_BUILD_NO_SORTBYPTYPE_PROCESS",
				"ASSIMP_BUILD_NO_GENUVCOORDS_PROCESS",
				"ASSIMP_BUILD_NO_TRANSFORMTEXCOORDS_PROCESS",
				"ASSIMP_BUILD_NO_FINDINSTANCES_PROCESS",
				"ASSIMP_BUILD_NO_OPTIMIZEMESHES_PROCESS",
				"ASSIMP_BUILD_NO_OPTIMIZEGRAPH_PROCESS",
				"ASSIMP_BUILD_NO_SPLITBYBONECOUNT_PROCESS",
				"ASSIMP_BUILD_NO_DEBONE_PROCESS",
				"ASSIMP_BUILD_NO_EMBEDTEXTURES_PROCESS",
				"ASSIMP_BUILD_NO_GLOBALSCALE_PROCESS",
				"ASSIMP_BUILD_BOOST_WORKAROUND",

				"RAPIDJSON_HAS_STDSTRING"
			}

			filter "action:vs*"; do
				files {
					"**.natvis"
				}
			end

			filter "kind:SharedLib"; do
				defines {
					"ASSIMP_BUILD_DLL_EXPORT"
				}
			end

			usage "INTERFACE"; do
				includedirsafter {
					"include"
				}
				links {
					prj.name
				}
				filter { "kind:SharedLib" }; do
					defines {
						"ASSIMP_DLL",
					}
				end
			end
		end,
	},
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
			Ref = "v1.92.8-docking",
		},
		OnProject = function(prj)
			language "C++"
			kind "SharedLib"

			files {
				"imconfig.h",
				"imgui*.h",
				"imgui*.cpp",
				"imstb*.h",
				"backends/imgui_impl_glfw.h",
				"backends/imgui_impl_glfw.cpp",
				"backends/imgui_impl_vulkan.h",
				"backends/imgui_impl_vulkan.cpp",
			}

			-- While setting up ImGui, for reference
			files {
				"examples/example_glfw_vulkan/main.cpp",
			}
			filter { "files:examples/example_glfw_vulkan/main.cpp" }; do
				excludefrombuild "On"
			end; filter {}

			includedirs {
				".",
				"backends"
			}

			filter "action:vs*"; do
				files {
					"**.natvis",
				}
			end

			uses {
				"Glfw",
				"VulkanSdk",
			}

			-- Tight coupling with Core, but this is ok since everything that uses core will also use ImGui
			includedirsafter {
				'%{!wks.projects["Core"].basedir}/Public'
			}
			usage "PUBLIC"; do
				defines {
					'IMGUI_USER_CONFIG=<Core/ImGui/ImGuiConfig.h>'
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
	VulkanSdk = {
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
					"dxcompiler.dll",
					"spirv-cross-reflect.dll"
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
					"spirv-cross-cppd",
					"spirv-cross-cored",
					"spirv-cross-glsld",
					"spirv-cross-reflectd",
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
