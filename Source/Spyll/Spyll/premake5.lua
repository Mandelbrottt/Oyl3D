local Project = require "Project"
local Package = require "Package"
local Packages = require "Spyll.Packages"

group "Spyll"

project "Spyll.Core"; do
	filename("%{prj.name}")

	language "C++"
	kind(premake.STATICLIB)

	Spyll.CommonCppSettings()

	files {
		"%{prj.location}/**.cpp",
		"%{prj.location}/**.h",
		"%{prj.location}/**.hpp",
		"%{prj.location}/**.inl",
		"%{prj.location}/**.ixx",
	}

	includedirs {
		"%{prj.location}",
	}

	defines {
		Project.InsideProjectMacro()
	}

	Package.Include(Packages.ClangTooling)
end
