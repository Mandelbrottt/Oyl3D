#pragma once

#include <Core/Modules/ModuleInterface.h>

#include <Rendering/Rendering.h>

namespace Oyl
{
	class IImGuiImpl;
	class IImGuiImplFactory;

	class IImGuiModule
		: public IImGuiRendererFactory,
		  public IModuleInterface
	{
	public:
		virtual
		~IImGuiModule() = 0;

		virtual
		void
		RegisterImGuiImplFactory(IImGuiImplFactory& a_factory) = 0;

		virtual
		void
		UnregisterImGuiImplFactory(IImGuiImplFactory& a_factory) = 0;

		virtual
		UniquePtr<IImGuiImpl>
		CreateImGuiImpl(RenderingApi a_api) = 0;
	};

	inline
	IImGuiModule::~IImGuiModule() {}
}
