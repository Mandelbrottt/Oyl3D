---@type Packages
return {
	SpdLog = {
		Git = {
			Url = "https://github.com/gabime/spdlog.git",
			Ref = "v1.11.0"
		},
	},
	TracyClient = {
		Git = {
			Url = "https://github.com/wolfpld/tracy.git",
			Ref = "v0.10"
		},
	},
	NlohmannJson = {
		Git = {
			Url = "https://github.com/nlohmann/json.git",
			Ref = "v3.6.1",
			Sparse = {
				"/*",
				"!/test",
				"!/benchmarks",
				"!/single_include",
			}
		},
	},
	YamlCpp = {
		Git = {
			Url = "https://github.com/jbeder/yaml-cpp.git",
			Ref = "0.8.0",
		},
	},

	GLFW = {
		Git = {
			Url = "https://github.com/glfw/glfw.git",
			Ref = "3.3.8"
		},
	},
	ImGui = {
		Git = {
			Url = "https://github.com/ocornut/imgui.git",
			Ref = "v1.92.0",
		},
	},
	-- ClangTooling = {
	-- 	Remote = {
	-- 		Url = (function()
	-- 			if (os.host() == premake.WINDOWS) then
	-- 				return "https://dl.dropbox.com/scl/fi/uixomsswnbbrlg3kmpqsq/llvm.txz?rlkey=4r754os6t6igz8h9a6upuhpp0"
	-- 			end
	-- 			return ""
	-- 		end)()
	-- 	},
	-- },
	Vulkan = {
		Local = {
			Path = assert(
				os.getenv("VULKAN_SDK"),
				"Missing Vulkan SDK! Please install the Vulkan SDK (https://vulkan.lunarg.com/sdk/home)!"
			)
		}
	}
}