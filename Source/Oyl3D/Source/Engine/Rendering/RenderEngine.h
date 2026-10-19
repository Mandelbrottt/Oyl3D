#pragma once

#include <Core/Containers/UniquePtr.h>
#include <Core/Preprocessor/Common.h>

#include "Device.h"
#include "PresentTarget.h"

namespace Oyl
{
	struct IPresentTargetFactory;

	struct IDeviceFactory;

	class RENDERING_EXPORT RenderEngine
	{
		RenderEngine();
		~RenderEngine();

		NO_COPY(RenderEngine);
		DEFAULT_MOVE(RenderEngine);

	public:
		static
		RenderEngine&
		Get();

		DeviceHandle
		CurrentDevice();

		void
		RegisterDevice(const DeviceHandle& a_device);

		void
		UnregisterDevice(const DeviceHandle& a_device);

	private:
		struct Impl;
		Impl* m_impl;
	};

	struct IPresentTargetFactory
	{
		virtual
		~IPresentTargetFactory() = 0;

		virtual
		PresentTargetHandle
		CreatePresentTarget(IApplicationWindow& a_window) = 0;
	};

	inline
	IPresentTargetFactory::~IPresentTargetFactory() {}

	struct IDeviceFactory
	{
		virtual
		~IDeviceFactory() = 0;

		virtual
		DeviceHandle
		CreateDevice(IPresentTarget& a_presentTarget) = 0;
	};

	inline
	IDeviceFactory::~IDeviceFactory() {}
}
