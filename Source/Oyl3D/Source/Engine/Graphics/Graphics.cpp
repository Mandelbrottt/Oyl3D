#include "Graphics.h"

#include <Core/Logging/Logging.h>

namespace Oyl
{
	IApplicationWindow*
	GraphicsModule::CreateWindow(const ApplicationWindowCreateParams& a_params)
	{
		OYL_UNUSED(a_params);
		LogDebug("CreateWindow!");
		return nullptr;
		//return TODO_IMPLEMENT_ME;
	}
}

OYL_MODULE_DECLARE(Graphics, Oyl::GraphicsModule)
