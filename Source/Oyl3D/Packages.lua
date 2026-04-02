local Config = require "Config"
local Package = require "Package"

---@type WorkspacePackage.List
local Packages = {
	Glfw = {
		Language = premake.C,
		Kind = premake.SHAREDLIB,
		Include = { "include", },
		OnProject = function(package)
			defines { "_CRT_SCURE_NO_WARNINGS" }
			removefiles { "**" }
			files {
				"include/GLFW/*",
				"src/glfw_config.h",
				"src/context.c",
				"src/init.c",
				"src/input.c",
				"src/monitor.c",
				"src/vulkan.c",
				"src/window.c"
			}
			filter "system:windows"; do
				systemversion "latest"
				files {
					"src/win32*.c",
					"src/wgl_context.c",
					"src/egl_context.c",
					"src/osmesa_context.c"
				}
				defines { "_GLFW_WIN32" }
			end
			filter "kind:StaticLib"; do
				defines {}
			end
			filter "kind:SharedLib"; do
				defines { "_GLFW_BUILD_DLL" }
			end
		end,
		OnDepend = function(package)
			if (package.Kind == premake.SHAREDLIB) then
				filter "platforms:*Editor*"; do
					defines { "GLFW_DLL" }
				end
			end
		end
	},
	ImGui = {
		Kind = premake.STATICLIB, -- ImGui SharedLib support is quite involved, so don't bother for now
		Include = { "." },
		OnProject = function(package)
			removefiles { "**" }
			files {
				"imconfig.h",
				"imgui*.h",
				"imgui*.cpp",
				"imstb*.h",
			}
		end
	},
	NlohmannJson = {
		Include = { "include" },
	},
	YamlCpp = {
		Kind = premake.SHAREDLIB,
		Include = { "Include" },
		OnProject = function(package)
			filter "kind:StaticLib"; do
				defines { "YAML_CPP_STATIC_DEFINE" }
			end
			filter "kind:SharedLib"; do
				defines { "yaml_cpp_EXPORTS" }
			end
		end
	},
	SpdLog = {
		Kind = premake.SHAREDLIB,
		Source = { "src" },
		Include = { "include" },
		OnProject = function(package)
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
			}
			filter "kind:SharedLib"; do
				defines {
					"spdlog_EXPORTS",
					"SPDLOG_SHARED_LIB",
					"FMT_EXPORT",
					"FMT_SHARED"
				}
			end
		end,
		OnDepend = function(package)
			defines {
				"SPDLOG_COMPILED_LIB",
				"_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING", -- Silence warning
			}
			if (package.Kind == premake.SHAREDLIB) then
				filter "platforms:*Editor*"; do
					defines {
						"SPDLOG_SHARED_LIB"
					}
				end
			end
		end
	},
	TracyClient = {
		Kind = premake.SHAREDLIB,
		Include = { "public" },
		OnProject = function(package)
			files {
				"public/TracyClient.cpp"
			}
			removeconfigurations {
				Config.Configurations.Debug,
				Config.Configurations.Development,
				Config.Configurations.Distribution
			}
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
			filter("configurations:not " .. Config.Configurations.Profile); do
				excludefrombuild "On"
			end
		end,
		OnDepend = function(package)
			if (package.Kind == premake.SHAREDLIB) then
				filter "platforms:*Editor*"; do
					defines {
						"TRACY_IMPORTS",
					}
				end
			end
			filter("configurations:" .. Config.Configurations.Profile); do
				defines {
					"TRACY_ENABLE",
					"TRACY_DELAYED_INIT",
					"TRACY_MANUAL_LIFETIME",
					"TRACY_NO_SYSTEM_TRACING",
				}
			end
		end
	},
	Vulkan = {
		Include = { "Include" },
		LibDirs = { "Lib" },
		Libs = { "vulkan-1" },
	}
}

Package.InitWorkspacePackages(Packages)

return Packages