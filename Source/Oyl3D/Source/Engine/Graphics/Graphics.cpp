#include "Graphics.h"

#include <Core/Logging/Logging.h>
#include <Core/Modules/ModuleManager.h>

namespace Oyl
{
	class GraphicsModule : public IGraphicsModule
	{
	public:
		void
		OnStartModule() override
		{
			ModuleManager::LoadModulePointer("GlfwGraphics");
		}

		void
		OnStopModule() override
		{
			ModuleManager::Get().UnloadModule("GlfwGraphics");
		}

		IApplicationWindow*
		CreateWindow(const ApplicationWindowCreateParams& a_params) override
		{
			// TODO: Load module based on user requested api
			auto* glfwGraphics = ModuleManager::GetModulePointer<IGraphicsModule>("GlfwGraphics");
			if (glfwGraphics)
				return glfwGraphics->CreateWindow(a_params);

			LogError("Failed to create window!");
			return nullptr;
		}
	};

	OYL_MODULE_DECLARE(Graphics, GraphicsModule)
}
