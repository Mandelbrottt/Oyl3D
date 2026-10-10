#pragma once

#include <Core/Application/ApplicationWindow.h>
#include <Core/Modules/ModuleInterface.h>

namespace Oyl
{
	class IGraphicsModule : public IModuleInterface
	{
	public:
		virtual
		~IGraphicsModule() = 0;

		virtual
		IApplicationWindow*
		CreateWindow(const ApplicationWindowCreateParams& a_params) = 0;

		bool
		SupportsHotReload() override
		{
			return false;
		}
	};

	inline
	IGraphicsModule::~IGraphicsModule() {}
}
