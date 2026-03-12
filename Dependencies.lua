local Config = require "Config"
local Engine = require "Engine"

require "Package"

Oyl.Packages.SpdLog = {
    Git = {
        Url = "https://github.com/gabime/spdlog.git",
        Tag = "v1.11.0"
    },
    Kind = premake.SHAREDLIB,
    Files = { "/include/", "/src/" },
    CustomProperties = function(package)
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
        filter "kind:SharedLib"
            defines { 
                "spdlog_EXPORTS",
                "SPDLOG_SHARED_LIB",
                "FMT_EXPORT",
                "FMT_SHARED"
            }
    end,
    DependantProperties = function(package)
        defines {
            "SPDLOG_COMPILED_LIB"
        }
        if (package.Kind == premake.SHAREDLIB) then
            Engine.FilterEditor()
            do
                defines {
                    "SPDLOG_SHARED_LIB"
                }
            end
        end
    end
}

Oyl.Packages.TracyClient = {
        Git = {
            Url = "https://github.com/wolfpld/tracy.git",
            Tag = "v0.10"
        },
        Kind = premake.SHAREDLIB,
        Files = { "public/" },
        IncludeDirs = { "public/ "},
        CustomProperties = function(package)
            removefiles { "**" }
            files {
                "public/TracyClient.cpp",
                "public/tracy/tracy.hpp"
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

            filter "kind:SharedLib"
                defines {
                    "TRACY_EXPORTS"
                }
        end,
        DependantProperties = function(package)
            if (package.Kind == premake.SHAREDLIB) then
                Engine.FilterEditor()
                do
                    defines {
                        "TRACY_IMPORTS",
                    }
                end
            end

            filter("configurations:" .. Config.Configurations.Profile)
                defines {
                    "TRACY_ENABLE",
                    "TRACY_DELAYED_INIT",
                    "TRACY_MANUAL_LIFETIME",
                    "TRACY_NO_SYSTEM_TRACING",
                }
            filter("configurations:not " .. Config.Configurations.Profile)
                removelinks { package.Name }
        end
}

Oyl.Packages.NlohmannJson = {
        Git = {
            Url = "https://github.com/nlohmann/json.git",
            Tag = "v3.6.1"
        },
        Kind = "Utility",
        Files = { "/include/" },
}

Oyl.Packages.YamlCpp = {
    Git = {
        Url = "https://github.com/jbeder/yaml-cpp.git",
        Revision = "c73ee34704c512ebe915b283645aefa9f424a22f",
    },
    Kind = premake.SHAREDLIB,
    Files = { 
        "/include/",
        "/src/"
    },
    CustomProperties = function(package)
        filter "kind:StaticLib"
            defines { "YAML_CPP_STATIC_DEFINE" }
        filter "kind:SharedLib"
            defines { "yaml_cpp_EXPORTS" }
    end
}

Oyl.Packages.GLFW = {
        Git = {
            Url = "https://github.com/glfw/glfw.git",
            Tag = "3.3.8"
        },
        Kind = premake.SHAREDLIB,
        Files = { "/include/", "/src/", },
        CustomProperties = function(package)
            language "C"
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
                defines { }
            end
            filter "kind:SharedLib"; do
                defines { "_GLFW_BUILD_DLL" }
            end
        end,
        DependantProperties = function(package)
            if (package.Kind == premake.SHAREDLIB) then
                Engine.FilterEditor(); do
                    defines { "GLFW_DLL" }
                end
            end
        end
}

Oyl.Packages.ImGui = {
    Git = {
        Url = "https://github.com/ocornut/imgui.git",
        Revision = "e2cede6542d2d6c83598d4d34dc51de84aeb282f"
    },
    Kind = premake.STATICLIB, -- ImGui SharedLib support is quite involved, so don't bother for now
    Files = { "/examples/", "imconfig.h", "/imgui*.h", "/imgui*.cpp", "/imstb*.h" },
    IncludeDirs = { "." },
    CustomProperties = function(package)
        -- For now we want to include examples on disk, but don't compile them.
        -- We will be manually including and compiling them later
        removefiles { "examples/**" }
    end
}

Oyl.Packages.ClangTooling = {
    Kind = premake.NONE,
    Archive = {
        Url = (function()
            if (os.host() == premake.WINDOWS) then
                return "https://dl.dropbox.com/scl/fi/h0oijr7uq92at4a0ep4wn/llvm.txz?rlkey=4oobdxdiod6eotm4pl1h2v4fw"
            end
            return ""
        end)()
    },
    IncludeDirs = { "include" },
    LibDirs = { "lib" },
    Libs = {},
    OnFetch = function(package)
        -- Return basename of all .lib files in lib folder (lib/someLib.lib -> someLib)
        local libPaths = os.matchfiles("lib/*.lib")
        for _, libPath in ipairs(libPaths) do
            table.insert(package.Libs, path.getbasename(libPath))
        end
    end
}

Oyl.Packages.Vulkan = {
    GenerateProject = false,
    ProjectDir = assert(os.getenv("VULKAN_SDK"), "Missing Vulkan SDK! Please install the Vulkan SDK!"),
    IncludeDirs = { "Include" },
    LibDirs = { "Lib" },
    Libs = { "vulkan-1" },
}
