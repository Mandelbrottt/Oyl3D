Dependencies = {
    {
        Git = {
            Url = "https://github.com/jbeder/yaml-cpp.git",
            Revision = "c73ee34704c512ebe915b283645aefa9f424a22f",
        },
        Kind = "SharedLib",
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
    {
        Name = "NlohmannJson",
        Git = {
            Url = "https://github.com/nlohmann/json.git",
            Revision = "v3.11.2"
        },
        Kind = "Utility",
        Files = { "/include/" },
    },
    {
        Name = "GLFW",
        Git = {
            Url = "https://github.com/glfw/glfw.git",
            Revision = "3.3.8"
        },
        Kind = "SharedLib",
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
    {
        Name = "ImGui",
        Git = {
            Url = "https://github.com/ocornut/imgui.git",
            Revision = "e2cede6542d2d6c83598d4d34dc51de84aeb282f"
        },
        Kind = "StaticLib",
        Files = { "/examples/", "imconfig.h", "/imgui*.h", "/imgui*.cpp", "/imstb*.h" },
        IncludeDirs = { "." },
        CustomProperties = function()
            -- For now we want to include examples on disk, but don't compile them.
            -- We will be manually including and compiling them later
            removefiles { "examples/**" }
        end
    },
    {
        Name = "SpdLog",
        Git = {
            Url = "https://github.com/gabime/spdlog.git",
            Revision = "v1.11.0"
        },
        Kind = "SharedLib",
        Files = { "/include/", "/src/" },
        CustomProperties = function()
            defines {
                "SPDLOG_COMPILED_LIB",
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
    }
}