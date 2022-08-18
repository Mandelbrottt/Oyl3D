Dependencies = {
    {
        Git = "https://github.com/jbeder/yaml-cpp.git",
        Revision = "c73ee34704c512ebe915b283645aefa9f424a22f",
        Linking = "StaticLib",
        CustomProperties = function()
            removefiles { "util**", "test**" }
            filter "kind:StaticLib"
                defines { "YAML_CPP_STATIC_DEFINE" }
            filter "kind:SharedLib"
                defines { "yaml_cpp_EXPORTS" }
        end
    },
}