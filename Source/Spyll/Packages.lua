---@type Package.Projects
return {
	ClangTooling = {
		Language = premake.CPP,
		Include = { "include" },
		LibDirs = { "lib" },
		OnProject = function (package)
			-- need direct paths to libs instead of finding through libdirs, for some reason
			package.Libs = os.matchfiles(path.join(package.PackageDir, "lib", "*.lib"))
		end,
		OnDepend = function (package)
			runtime "Release"
			defines {
				"_ITERATOR_DEBUG_LEVEL=0"
			}
		end
	}
}
