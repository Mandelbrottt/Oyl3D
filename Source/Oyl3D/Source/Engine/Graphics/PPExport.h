#pragma once

#include <Core/PPExport.h>

#if !defined(GRAPHICS_EXPORT)
#	if defined(GRAPHICS_SHAREDLIB)
#		if defined(OYL_WITHIN_MODULE_GRAPHICS)
#			define GRAPHICS_EXPORT __OYL_EXPORT_ATTR
#		else
#			define GRAPHICS_EXPORT __OYL_IMPORT_ATTR
#		endif
#	else
#		define GRAPHICS_STATICLIB
#		define GRAPHICS_EXPORT
#	endif
#endif
