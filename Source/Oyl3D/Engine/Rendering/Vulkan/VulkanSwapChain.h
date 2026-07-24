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
		class Semaphore;
		class Fence;
	}
}

namespace Oyl::Rendering::Vulkan
{
	class Device;

	class OYL_RENDERING_API SwapChain : public Rendering::SwapChain
	{
	public:
		SwapChain();

		struct CreateParams
		{
			const Window* window;
			const Device* device;

			const vk::raii::Semaphore* semaphore = nullptr;
			const vk::raii::Fence* fence = nullptr;
		};

		explicit
		SwapChain(const CreateParams& a_params);

		SwapChain(SwapChain&& a_other) noexcept;
		SwapChain&
		operator =(SwapChain&& a_other) noexcept;

		virtual
		~SwapChain();

		void
		Destroy() override;

		bool
		IsValid() const override;

		void
		Recreate() override;

		bool
		AcquireNextImage() override;

		uint32
		GetCurrentImageIndex() const override;

		const vk::raii::SwapchainKHR&
		GetVkSwapChain() const;

		const std::vector<vk::Image>&
		GetVkImages() const;

		vk::Image
		GetCurrentVkImage() const;

		const std::vector<vk::raii::ImageView>&
		GetVkImageViews() const;

		const vk::raii::ImageView&
		GetCurrentVkImageView() const;

		const vk::SurfaceFormatKHR&
		GetVkSurfaceFormat() const;

		const vk::Extent2D&
		GetVkExtent() const;

		const vk::raii::Semaphore*
		GetVkSemaphore() const;

		void
		SetVkSemaphore(const vk::raii::Semaphore* a_semaphore) const;

		const vk::raii::Fence*
		GetVkFence() const;

		void
		SetVkFence(const vk::raii::Fence* a_fence) const;

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}
