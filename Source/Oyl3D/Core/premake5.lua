group "Engine"

project "Oyl.Core"; do
	language(premake.CPP)
	kind(premake.SHAREDLIB)

	-- buildtargetcustomizations {
	-- 	path.join("%{wks.location}", "Oyl3D.Cpp.Reflection.targets")
	-- }

	links {
		"SpdLog",
		"TracyClient",
		"Vulkan",
		"Oyl.Spyll",
	}
end
