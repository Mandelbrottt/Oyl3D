#pragma once

#include <Core/Containers/UniquePtr.h>

#include <Rendering/RenderingEnums.h>
#include <Rendering/Hardware/RdoImage.h>

namespace Oyl
{
	class IImGuiRenderer;
	class IApplicationWindow;
	class IRdoCommandBuffer;

	class IImGuiImpl
	{
	public:
		virtual
		~IImGuiImpl() = 0;

		virtual
		void
		Init(IImGuiRenderer& a_imGuiRenderer, IApplicationWindow& a_mainWindow) = 0;

		virtual
		void
		Update() = 0;

		virtual
		void
		Render(IRdoCommandBuffer& a_commandBuffer) = 0;

		virtual
		void
		Shutdown() = 0;

		virtual
		const RdoImageHandle&
		GetRenderedImageHandle() const = 0;
	};

	inline
	IImGuiImpl::~IImGuiImpl() {}

	class IImGuiImplFactory
	{
	public:
		virtual
		~IImGuiImplFactory() = 0;

		virtual
		bool
		SupportsApi(RenderingApi a_api) = 0;

		virtual
		UniquePtr<IImGuiImpl>
		CreateImGuiImpl() = 0;
	};

	inline
	IImGuiImplFactory::~IImGuiImplFactory() {}
}
