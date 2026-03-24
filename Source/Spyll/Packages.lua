---@type WorkspacePackage.List
return {
	ClangTooling = {
		Language = premake.CPP,
		Include = { "include" },
		LibDirs = { "lib" },
		OnInit = function(package)
			-- need direct paths to libs instead of finding through libdirs, for some reason
			package.Libs = os.matchfiles(path.join(package.PackageDir, "lib", "*.lib"))
		end,
		OnDepend = function(package)
			runtime "Release"
			defines {
				"_ITERATOR_DEBUG_LEVEL=0"
			}
		end
	},
	GoogleTest = {
		Language = premake.CPP,
		Kind = premake.STATICLIB,
		Include = { "googletest/include" },
		OnProject = function(package)
			includedirs { "googletest" }
			files {
				"googletest/src/gtest-all.cc",
			}
			filter("kind:" .. premake.SHAREDLIB); do
				defines {
					"GTEST_CREATE_SHARED_LIBRARY=1"
				}
			end
		end,
		OnDepend = function(package)
			if (package.Kind == premake.SHAREDLIB) then
				defines {
					"GTEST_LINKED_AS_SHARED_LIBRARY=1"
				}
			end
		end
	},
	NlohmannJson = {
		Include = { "include" },
	},
}
