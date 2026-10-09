#pragma once

#include <Core/Application/ApplicationWindow.h>
#include <Core/Modules/ModuleManager.h>

#include "PPExport.h"

namespace Oyl
{
	class GRAPHICS_EXPORT GraphicsModule : public IModuleInterface
	{
	public:
		virtual
		~GraphicsModule() = default;

		IApplicationWindow*
		CreateWindow(const ApplicationWindowCreateParams& a_params);
	};
}
