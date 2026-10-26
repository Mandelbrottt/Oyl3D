#pragma once

#include <Core/Modules/ModuleInterface.h>

#include "Application/Application.h"

namespace Oyl
{
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
		int
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
