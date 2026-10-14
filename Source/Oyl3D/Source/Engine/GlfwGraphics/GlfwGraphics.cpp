#include <Core/Logging/Logging.h>
#include <Core/Modules/ModuleManager.h>

#include <GLFW/glfw3.h>

#include <Graphics/Graphics.h>

#include "GlfwApplicationWindow.h"

namespace Oyl
{
	class GlfwGraphicsModule : public IGraphicsModule
	{
	public:
		virtual
		~GlfwGraphicsModule() = default;

		void OnStartModule() override
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
		}

		void OnStopModule() override
		{
			glfwTerminate();
		}

		IApplicationWindow*
		CreateWindow(const ApplicationWindowCreateParams& a_params) override
		{
			if (m_initialized == GLFW_FALSE)
				return nullptr;

			return new GlfwApplicationWindow(a_params);
		}

		static
		void
		GlfwErrorCallback(int a_error, const char* a_description)
		{
			LogError("GLFW ERROR ({}): \"{}\"", a_error, a_description);
		}

	private:
		int m_initialized = GLFW_FALSE;
	};

	OYL_MODULE_DECLARE(GlfwGraphics, GlfwGraphicsModule)
}
