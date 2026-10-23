#include <Application/Application.h>

#include <Core/Modules/ModuleManager.h>

#include <ImGui/backends/imgui_impl_glfw.h>

#include "GlfwApplicationWindow.h"

namespace Oyl
{
	class GlfwModule final
		: public IApplicationWindowFactory,
		  public IModuleInterface
	{
	public:
		virtual
		~GlfwModule() = default;

		void
		OnStartModule() override
		{
			m_initialized = glfwInit();
			if (m_initialized == GLFW_FALSE)
			{
				const char* err;
				glfwGetError(&err);
				LogError("Failed to initialize GLFW: {}", err);
				return;
			}
			glfwSetErrorCallback(GlfwErrorCallback);

			auto* applicationModule = ModuleManager::GetModulePointer<IApplicationModule>("Application");
			applicationModule->RegisterApplicationWindowFactory(*this);
			applicationModule->CurrentApplication()->RegisterOnUpdateCallback([]
			{
				OYL_PROFILE_SCOPE("glfwPollEvents()");
				glfwPollEvents();
			});
		}

		void
		OnStopModule() override
		{
			glfwTerminate();
		}

		static
		void
		GlfwErrorCallback(int a_error, const char* a_description)
		{
			LogError("GLFW ERROR ({}): \"{}\"", a_error, a_description);
		}

		IApplicationWindow*
		CreateApplicationWindow(const ApplicationWindowCreateParams& a_params) override
		{
			if (m_initialized == GLFW_FALSE)
				return nullptr;

			return new GlfwApplicationWindow(a_params);
		}

	private:
		int m_initialized = GLFW_FALSE;
	};

	OYL_MODULE_DECLARE(GlfwModule, GlfwModule)
}
