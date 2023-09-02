RearmModule({
    Group = "Editor",
    Language = premake.CPP,
    Kind = premake.SHAREDLIB,
    Dependencies = {
        "Core",
        "ImGui",
        "YamlCpp",
    },
})