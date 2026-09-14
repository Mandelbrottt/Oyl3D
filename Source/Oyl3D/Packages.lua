local Config = require "Config"
local Package = require "Package"
local PackageCache = require "Packages"

---@type { [string]: { OnProject: fun(prj), OnDepend: fun(prjcfg, packagecfg)? } }
local Packages = {
	Glfw = {
		OnProject = function(prj)
			language "C"
			kind "SharedLib"
			filter "platforms:not *Editor*"; do
				kind "StaticLib"
			end; filter {}

			files {
				"include/GLFW/**",
				"src/**",
			}

			includedirs {
				"include/GLFW"
			}

			packageincludedir "include/GLFW"

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
		end,
		OnDepend = function(prjcfg, packagecfg)
			if packagecfg.kind == "SharedLib" then
				defines { "GLFW_DLL" }
			end
		end
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

			packageincludedir "."

			filter "action:vs*"; do
				files {
					"**.natvis"
				}
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

			packageincludedir "include/nlohmann"

			filter "action:vs*"; do
				files {
					"**.natvis"
				}
			end
		end
	},
	YamlCpp = {
		OnProject = function(prj)
			language "C++"
			kind "SharedLib"
			filter "platforms:not *Editor*"; do
				kind "StaticLib"
			end; filter {}

			files {
				"src/**",
				"include/**",
			}

			includedirs {
				".",
				"include/yaml-cpp"
			}

			packageincludedir "include/yaml-cpp"

			filter "kind:StaticLib"; do
				defines { "YAML_CPP_STATIC_DEFINE" }
			end
			filter "kind:SharedLib"; do
				defines { "yaml_cpp_EXPORTS" }
			end
		end
	},
	SpdLog = {
		OnProject = function(prj)
			language "C++"
			kind "SharedLib"

			files {
				"src/**",
				"include/**"
			}

			includedirs {
				"include"
			}

			packageincludedir "include/spdlog"

			defines {
				"SPDLOG_COMPILED_LIB",
				"SPDLOG_LEVEL_NAMES={" ..
				[[spdlog::string_view_t("TRACE", 5),]] ..
				[[spdlog::string_view_t("DEBUG", 5),]] ..
				[[spdlog::string_view_t("INFO", 4),]] ..
				[[spdlog::string_view_t("WARNING", 7),]] ..
				[[spdlog::string_view_t("ERROR", 5),]] ..
				[[spdlog::string_view_t("FATAL", 5),]] ..
				[[spdlog::string_view_t("OFF", 3),]] ..
				"}",
				"SPDLOG_SHORT_LEVEL_NAMES={" ..
				[["T", "D", "I", "W", "E", "F", "O"]] ..
				"}",
				"_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING",
				"FMT_UNICODE=0",
				"FMT_USE_CONSTEVAL=0",
			}
			filter "kind:SharedLib"; do
				defines {
					"spdlog_EXPORTS",
					"SPDLOG_SHARED_LIB",
					"FMT_LIB_EXPORT",
					"FMT_SHARED",
				}
			end
		end,
		OnDepend = function(prjcfg, packagecfg)
			defines {
				"SPDLOG_COMPILED_LIB",
				"_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING", -- Silence warning
				"FMT_UNICODE=0",
				"FMT_USE_CONSTEVAL=0",
			}
			if packagecfg.kind == "SharedLib" then
				defines {
					"SPDLOG_SHARED_LIB",
					"FMT_SHARED"
				}
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
				"public/**"
			}

			includedirs {
				"public"
			}

			packageincludedir "public/tracy"

			defines {
				"TRACY_ENABLE",
				"TRACY_DELAYED_INIT",
				"TRACY_MANUAL_LIFETIME",
				"TRACY_NO_SAMPLING",
				"TRACY_NO_SYSTEM_TRACING",
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
		end,
		OnDepend = function(prjcfg, packagecfg)
			if prjcfg.buildcfg == Config.Configurations.Profile then
				defines {
					"TRACY_ENABLE",
					"TRACY_DELAYED_INIT",
					"TRACY_MANUAL_LIFETIME",
					"TRACY_NO_SYSTEM_TRACING",
				}
			end
			if packagecfg.kind == "SharedLib" then
				defines {
					"TRACY_IMPORTS",
				}
			end
		end
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

			libdirs {
				"Lib"
			}

			filter { "configurations:*" .. Config.Configurations.Distribution .. "*" }; do
				local sharedLibsToCopy = {
					"dxcompiler.dll"
				}
				for _, sharedLib in ipairs(sharedLibsToCopy) do
					local inFile = path.join(PackageCache.Vulkan.Local.Path, path.join("Bin", sharedLib))
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
		end,
		OnDepend = function(prjcfg, packagecfg)
			defines {
				"VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1",
				"VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1",
				"VULKAN_HPP_HANDLE_ERROR_OUT_OF_DATE_AS_SUCCESS=1"
			}

			externalincludedirs {
				path.join(packagecfg.basedir, "Include")
			}
		end
	},
	["Spyll.Core"] = {
		OnProject = function(package)
			basedir(path.join(Config.SourceDir, "Spyll/Tool/Core"))
			os.chdir(premake.api.scope.project.basedir)

			language "C++"
			kind "StaticLib"

			files {
				"**.cpp",
				"**.h",
				"**.hpp",
			}

			-- Project settings set by premake5.lua in basedir

			filter "action:vs*"; do
				-- TODO: Make dependant on variable name in root Packages.lua
				local clangNatvisPattern = path.join(Config.PackageCacheDir, "ClangTooling", "**.natvis")
				files {
					"**.natvis",
					clangNatvisPattern
				}
			end
		end
	},
}

return Packages
