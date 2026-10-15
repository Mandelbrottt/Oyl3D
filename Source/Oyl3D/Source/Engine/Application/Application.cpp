#include "Application.h"

#include <Core/Logging/Logging.h>
#include <Core/Modules/ModuleManager.h>

namespace Oyl
{
	class ApplicationModule : public IApplicationModule
	{
	public:
		void
		RegisterApplication(IApplication* a_application) override
		{
			if (!m_currentApplication)
				m_currentApplication = a_application;
			else
				LogWarning("Failed to register application! An application has already been registered");
		}

		IApplication*
		CurrentApplication() const override
		{
			return m_currentApplication;
		}

		int RunApplication() override
		{
			if (m_applicationStarted)
				return 0;

			m_currentApplication->OnStart();
			int exitCode = 0;
			while (!m_currentApplication->IsExitRequested(&exitCode))
			{
				m_currentApplication->OnUpdate();

				for (auto& callback : m_currentApplication->GetOnUpdateCallbacks())
					callback();

				m_currentApplication->WaitIdle();
			}
			m_currentApplication->OnStop();
			return exitCode;
		}

		const Array<IApplicationWindowFactory*>&
		GetApplicationWindowFactories() const override { return m_appWindowFactories; }

		void
		RegisterApplicationWindowFactory(IApplicationWindowFactory& a_factory) override
		{
			auto iter = std::ranges::find(m_appWindowFactories, &a_factory);
			if (iter == m_appWindowFactories.end())
				m_appWindowFactories.push_back(&a_factory);
			else
				LogWarning("ApplicationWindowFactory already registered!");
		}

		void
		UnregisterApplicationWindowFactory(IApplicationWindowFactory& a_factory) override
		{
			auto iter = std::ranges::find(m_appWindowFactories, &a_factory);
			if (iter != m_appWindowFactories.end())
				m_appWindowFactories.erase(iter);
			else
				LogWarning("Trying to unregister ApplicationWindowFactory that hasn't been registered!");
		}

	private:
		IApplication* m_currentApplication = nullptr;
		Array<IApplicationWindowFactory*> m_appWindowFactories;

		bool m_applicationStarted = false;
	};

	OYL_MODULE_DECLARE(Application, ApplicationModule)
}
