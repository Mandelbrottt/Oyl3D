#pragma once

#include <vulkan/vulkan.hpp>

#include "Rendering/Fence.h"

namespace Oyl::Rendering::Vulkan
{
	using FenceHandle = ImplicitConversionWrapper<FenceHandle, vk::Fence>;

	class Device;

	class OYL_RENDERING_API Fence : public IFence
	{
	public:
		Fence();

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

		const FenceHandle&
		GetHandle() const override;

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}
