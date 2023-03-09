Dependencies = {
    {
        Git = {
            Url = "https://github.com/jbeder/yaml-cpp.git",
            Revision = "c73ee34704c512ebe915b283645aefa9f424a22f",
        },
        Kind = "StaticLib",
        -- IncludeDirs = { "include" },
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
            Revision = "bc889afb4c5bf1c0d8ee29ef35eaaf4c8bef8a5d"
        },
        -- IncludeDirs = { "include" },
        Files = { "/include/" },
        Kind = "None",
    },
}