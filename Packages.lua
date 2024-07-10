Packages = {
    YamlCpp = {
        Git = {
            Url = "https://github.com/jbeder/yaml-cpp.git",
            Revision = "c73ee34704c512ebe915b283645aefa9f424a22f",
        },
        Kind = premake.SHAREDLIB,
        Files = { 
            "/include/",
            "/src/"
        },
        CustomProperties = function()
            filter "kind:StaticLib"
                defines { "YAML_CPP_STATIC_DEFINE" }
            filter "kind:SharedLib"
                defines { "yaml_cpp_EXPORTS" }
        end
    },
    NlohmannJson = {
        Git = {
            Url = "https://github.com/nlohmann/json.git",
            Tag = "v3.6.1"
        },
        Kind = "Utility",
        Files = { "/include/" },
    },
    GLFW = {
        Git = {
            Url = "https://github.com/glfw/glfw.git",
            Tag = "3.3.8"
        },
        Kind = premake.SHAREDLIB,
        Files = { "/include/", "/src/", },
        CustomProperties = function()
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
            filter "system:windows"
                systemversion "latest"
                files {
                    "src/win32*.c",
                    "src/wgl_context.c",
                    "src/egl_context.c",
                    "src/osmesa_context.c"
                }
                defines { "_GLFW_WIN32" }
            filter "kind:StaticLib"
                defines { }
            filter "kind:SharedLib"
                defines { "_GLFW_BUILD_DLL" }
        end
    },
    ImGui = {
        Git = {
            Url = "https://github.com/ocornut/imgui.git",
            Revision = "e2cede6542d2d6c83598d4d34dc51de84aeb282f"
        },
        Kind = premake.STATICLIB,
        Files = { "/examples/", "imconfig.h", "/imgui*.h", "/imgui*.cpp", "/imstb*.h" },
        IncludeDirs = { "." },
        CustomProperties = function()
            -- For now we want to include examples on disk, but don't compile them.
            -- We will be manually including and compiling them later
            removefiles { "examples/**" }
        end
    },
    SpdLog = {
        Git = {
            Url = "https://github.com/gabime/spdlog.git",
            Tag = "v1.11.0"
        },
        Kind = premake.SHAREDLIB,
        Files = { "/include/", "/src/" },
        CustomProperties = function()
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
                "}"
            }
            filter "kind:StaticLib"
                defines { }
            filter "kind:SharedLib"
                defines { 
                    "spdlog_EXPORTS",
                    "SPDLOG_SHARED_LIB",
                    "FMT_EXPORT",
                    "FMT_SHARED"
                }
        end
    },
    TracyClient = {
        Git = {
            Url = "https://github.com/wolfpld/tracy.git",
            Tag = "v0.10"
        },
        Kind = premake.SHAREDLIB,
        Files = { "public/" },
        IncludeDirs = { "public/ "},
        CustomProperties = function()
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

            filter "kind:SharedLib"
                defines {
                    "TRACY_ENABLE",
                    "TRACY_EXPORTS",
                    "TRACY_DELAYED_INIT",
                    "TRACY_MANUAL_LIFETIME",
                    "TRACY_NO_SAMPLING",
                    "TRACY_NO_SYSTEM_TRACING",
                    "TRACY_ONLY_LOCALHOST", -- TODO: Fix only localhost at runtime
                }
        end
    },
    -- Clang = {
    --     Git = {
    --         Url = "https://github.com/llvm/llvm-project.git",
    --         Tag = "llvmorg-18.1.8"
    --     },
    --     Kind = premake.STATICLIB,
    --     Files = { "clang/", "clang-tools-extra/" },
    --     -- IncludeDirs = { "public/ "},
    --     CustomProperties = function()
            
    --     end
    -- },
    -- TracyServer = {
    --     Git = {
    --         Url = "https://github.com/wolfpld/tracy.git",
    --         Revision = "v0.10"
    --     },
    --     Kind = premake.WINDOWEDAPP,
    --     Files = { "profiler/", "server/", "public/", "imgui/", "nfd/", "zstd/" },
    --     IncludeDirs = { "profiler/", "public/" },
    --     CustomProperties = function()
    --         -- removefiles { "**" }
    --         -- files {
    --         --     "public/TracyClient.cpp",
    --         --     "public/tracy/tracy.hpp"
    --         -- }
    --     end
    -- }
}

local VULKAN_SDK = os.getenv("VULKAN_SDK")
if not VULKAN_SDK then
    premake.error("Missing Vulkan SDK! Please install the Vulkan SDK!")
end

Libraries = {
    Vulkan = {
        IncludeDirs = { VULKAN_SDK .. "/Include" },
        LibraryDirs = { VULKAN_SDK .. "/Lib" },
        Libraries = { "vulkan-1" }
    }
}