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