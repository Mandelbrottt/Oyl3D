---@type { [string]: Packages.Definition }
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
			Ref = "v3.6.1"
		},
	},
	YamlCpp = {
		Git = {
			Url = "https://github.com/jbeder/yaml-cpp.git",
			Ref = "c73ee34704c512ebe915b283645aefa9f424a22f",
		},
	},

	GLFW = {
		Git = {
			Url = "https://github.com/glfw/glfw.git",
			Tag = "3.3.8"
		},
	},
	ImGui = {
		Git = {
			Url = "https://github.com/ocornut/imgui.git",
			Revision = "e2cede6542d2d6c83598d4d34dc51de84aeb282f"
		},
	},
	ClangTooling = {
		Remote = {
			Url = (function()
				if (os.host() == premake.WINDOWS) then
					return "https://dl.dropbox.com/scl/fi/14cncqutbmo9t0v6u7e6c/llvm.txz?rlkey=8fsgqaswlnn9fzdusdqpdnz9v"
				end
				return ""
			end)()
		},
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

---@class (exact) Packages.Definitions.Git
---@field Git Packages.Definitions.GitTable

---@class (exact) Packages.Definitions.GitTable
---@field Url string
---@field Ref? string

---@class (exact) Packages.Definitions.Remote
---@field Remote Packages.Definitions.RemoteTable

---@class (exact) Packages.Definitions.RemoteTable
---@field Url string

---@class (exact) Packages.Definitions.Local
---@field Local Packages.Definitions.LocalTable

---@class (exact) Packages.Definitions.LocalTable
---@field Path string

---@alias Packages.Definition Packages.Definitions.Git | Packages.Definitions.Remote | Packages.Definitions.Local
