#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "Rendering/Fence.h"

namespace Oyl::Rendering::Vulkan
{
	class DeviceImpl;

	using FenceHandle = OpaqueHandleConvertible<FenceHandle, vk::Fence>;

	class OYL_RENDERING_API FenceImpl : public Rendering::FenceImpl
	{
	public:
		FenceImpl(nullptr_t) {}

		FenceImpl(const DeviceImpl& a_device);

		FenceImpl(FenceImpl&& a_other) noexcept;
		FenceImpl&
		operator =(FenceImpl&& a_other) noexcept;

		virtual
		~FenceImpl();

		void
		Destroy() override;

		bool
		IsValid() const override;

		void
		Reset() override;

		bool
		Wait(uint64 a_timeout = std::numeric_limits<uint64>::max()) override;

		const vk::raii::Fence&
		GetVkFence() const
		{
			return m_fence;
		}

		FenceHandle
		GetHandle() const
		{
			return *m_fence;
		}

		operator FenceHandle() const
		{
			return GetHandle();
		}

	protected:
		Rendering::FenceHandle
		GetHandleImpl() const override
		{
			return GetHandle();
		}

	private:
		const DeviceImpl* m_device = nullptr;
		vk::raii::Fence m_fence = nullptr;
	};

	using Fence = PImpl<FenceImpl>;
}
