#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "Rendering/Fence.h"

namespace Oyl::Rendering::Vulkan
{
	using FenceHandle = ImplicitConversionWrapper<FenceHandle, vk::Fence>;

	class Device;

	class OYL_RENDERING_API Fence : public IFence
	{
	public:
		Fence();

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

		FenceHandle
		GetHandle() const;

		const vk::raii::Fence&
		GetVkFence() const;

	protected:
		Rendering::FenceHandle
		GetHandleImpl() const override
		{
			return GetHandle();
		}

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}
