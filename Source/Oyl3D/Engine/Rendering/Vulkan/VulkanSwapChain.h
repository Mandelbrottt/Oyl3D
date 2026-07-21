#pragma once

#include <vulkan/vulkan_raii.hpp>

#include <Core/Math/Vector2.h>

#include "Rendering/SwapChain.h"

namespace Oyl
{
	class Window;
}

namespace vk
{
	struct Extent2D;
	struct SurfaceFormatKHR;

	namespace raii
	{
		class SurfaceKHR;
	}
}

namespace Oyl::Rendering::Vulkan
{
	class Device;

	struct SwapChainParams
	{
		const Window& window;
		const Device& device;
		const vk::raii::SurfaceKHR& surface;
	};

	class OYL_RENDERING_API SwapChain : public Rendering::SwapChain
	{
	public:
		SwapChain();

		explicit
		SwapChain(const SwapChainParams& a_params);

		SwapChain(const SwapChain&) = delete;
		SwapChain&
		operator =(const SwapChain&) = delete;

		SwapChain(SwapChain&& a_other) noexcept;
		SwapChain&
		operator =(SwapChain&& a_other) noexcept;

		virtual
		~SwapChain();

		bool
		Destroy() override;

		void
		Recreate() override;

		bool
		IsValid() const override;

		const vk::raii::SwapchainKHR&
		GetVkSwapChain() const;

		const std::vector<vk::Image>&
		GetVkImages() const;

		const std::vector<vk::raii::ImageView>&
		GetVkImageViews() const;

		const vk::SurfaceFormatKHR&
		GetVkSurfaceFormat() const;

		const vk::Extent2D&
		GetVkExtent() const;

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}
