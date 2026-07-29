#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "Rendering/Fence.h"

namespace Oyl::Rendering::Vulkan
{
	class Device;

	using FenceHandle = OpaqueHandleConvertible<FenceHandle, vk::Fence>;

	class OYL_RENDERING_API Fence : public IFence
	{
	public:
		Fence() = default;

		Fence(std::nullptr_t)
			: Fence() {}

		explicit
		Fence(const Device& a_device);

		Fence(Fence&& a_other) noexcept;
		Fence&
		operator =(Fence&& a_other) noexcept;

		virtual
		~Fence();

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
		const Device* m_device = nullptr;
		vk::raii::Fence m_fence = nullptr;
	};
}
