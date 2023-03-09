Dependencies = {
    {
        Git = {
            Url = "https://github.com/jbeder/yaml-cpp.git",
            Revision = "c73ee34704c512ebe915b283645aefa9f424a22f",
        },
        Kind = "StaticLib",
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
        Kind = "None",
        Files = { "/include/" },
    },
    {
        Name = "GLFW",
        Git = {
            Url = "https://github.com/glfw/glfw.git",
            Revision = "3.3.8"
        },
        Kind = "StaticLib",
        Files = { "/include/", "/src/", },
        CustomProperties = function()
            language "C"
            staticruntime "on"
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
        end
    },
}