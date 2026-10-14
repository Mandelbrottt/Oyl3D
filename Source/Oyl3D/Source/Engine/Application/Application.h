#pragma once

#include <Core/Containers/Array.h>
#include <Core/Modules/ModuleInterface.h>

#include "ApplicationWindow.h"

namespace Oyl
{
	using OnUpdateDelegate = Delegate<void()>;

	class IApplication
	{
		friend class ApplicationModule;

	public:
		virtual
		~IApplication() = 0;

	protected:
		virtual
		void
		OnStart() = 0;

		virtual
		void
		OnUpdate() = 0;

		virtual
		void
		OnStop() = 0;

		virtual
		void
		WaitIdle() = 0;

	public:
		virtual
		IApplicationWindow*
		CreateApplicationWindow(const ApplicationWindowCreateParams& a_params) = 0;

		virtual
		void
		RequestExit(int a_exitCode = 0) = 0;

		virtual
		void
		ForceExit(int a_exitCode = 0) = 0;

		virtual
		bool
		IsExitRequested(int* a_outExitCode = nullptr) = 0;

		virtual
		uint32
		TargetFramerate() = 0;

		virtual
		void
		SetTargetFramerate(uint32 a_target) = 0;

		virtual
		const Array<OnUpdateDelegate>&
		GetOnUpdateCallbacks() const = 0;

		virtual
		void
		RegisterOnUpdateCallback(OnUpdateDelegate a_callback) = 0;

		virtual
		void
		UnregisterOnUpdateCallback(OnUpdateDelegate a_callback) = 0;

	protected:
	};

	inline
	IApplication::~IApplication() {}

	class IApplicationModule : public IModuleInterface
	{
	public:
		virtual
		~IApplicationModule() = 0;

		virtual
		void
		RegisterApplication(IApplication* a_application) = 0;

		virtual
		IApplication*
		CurrentApplication() const = 0;

		virtual
		void
		RunApplication() = 0;

		virtual
		const Array<IApplicationWindowFactory*>&
		GetApplicationWindowFactories() const = 0;

		virtual
		void
		RegisterApplicationWindowFactory(IApplicationWindowFactory& a_factory) = 0;

		virtual
		void
		UnregisterApplicationWindowFactory(IApplicationWindowFactory& a_factory) = 0;

		bool
		SupportsHotReload() override
		{
			return false;
		}
	};

	inline
	IApplicationModule::~IApplicationModule() {}
}
