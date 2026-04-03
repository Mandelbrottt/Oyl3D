group "Executables"

project "Oyl.Spyll"; do
	language "C++"
	kind "ConsoleApp"

	links {
		"Spyll.Core",
		"Spyll.Clang",
	}
end
