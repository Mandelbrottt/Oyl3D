---@type Package.Projects
return {
	ClangTooling = {
		Language = premake.CPP,
		Include = { "include" },
		LibDirs = { "lib" },
		OnProject = function (package)
			package.Libs = table.translate(
				os.matchfiles(path.join(package.PackageDir, "*.lib")),
				function(value) return path.getbasename(value) end
			)
		end,
		OnDepend = function (package)
			runtime "Release"
			defines {
				"_ITERATOR_DEBUG_LEVEL=0"
			}
		end
	}
}
