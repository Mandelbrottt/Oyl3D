#pragma once

#include <Core/PPExport.h>

#if !defined(GLFWGRAPHICS_EXPORT)
#	if defined(GLFWGRAPHICS_SHAREDLIB)
#		if defined(OYL_WITHIN_MODULE_GLFWGRAPHICS)
#			define GLFWGRAPHICS_EXPORT __OYL_EXPORT_ATTR
#		else
#			define GLFWGRAPHICS_EXPORT __OYL_IMPORT_ATTR
#		endif
#	else
#		define GLFWGRAPHICS_STATICLIB
#		define GLFWGRAPHICS_EXPORT
#	endif
#endif
