---@type Packages
return {
	SpdLog = {
		Git = {
			Url = "https://github.com/gabime/spdlog.git",
			Ref = "v1.17.0",
		},
	},
	TracyClient = {
		Git = {
			Url = "https://github.com/wolfpld/tracy.git",
			Ref = "v0.13.1",
		},
	},
	NlohmannJson = {
		Git = {
			Url = "https://github.com/nlohmann/json.git",
			Ref = "v3.11.3",
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
			Ref = "yaml-cpp-0.9.0",
		},
	},

	GLFW = {
		Git = {
			Url = "https://github.com/glfw/glfw.git",
			Ref = "3.4",
		},
	},
	ImGui = {
		Git = {
			Url = "https://github.com/ocornut/imgui.git",
			Ref = "v1.92.8",
		},
	},
	ClangTooling = {
		Remote = {
			Url = (function()
				if (os.host() == premake.WINDOWS) then
					return "https://dl.dropbox.com/scl/fi/gaeyy54dlccesq7yuzvif/llvm.txz?rlkey=hxbbr6nv963xgclo36bokcr3o"
				end
				return ""
			end)()
		},
	},
	GoogleTest = {
		Git = {
			Url = "https://github.com/google/googletest.git",
			Ref = "v1.17.0",
		}
	},
	Vulkan = {
		Local = {
			Path = assert(
				os.getenv("VULKAN_SDK"),
				"Missing Vulkan SDK! Please install the Vulkan SDK (https://vulkan.lunarg.com/sdk/home)!"
			)
		}
	}
}