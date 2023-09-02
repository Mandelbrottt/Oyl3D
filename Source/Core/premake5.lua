RearmModule({
    Group = "Engine",
    Language = premake.CPP,
    Kind = premake.SHAREDLIB,
    Dependencies = {
        "SpdLog",
        "GLFW",
        "Vulkan"
    },
})