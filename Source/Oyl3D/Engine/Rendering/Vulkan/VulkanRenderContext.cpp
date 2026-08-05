#include "VulkanRenderContext.h"

#include <vulkan/vulkan_raii.hpp>

#include "VulkanDevice.h"
#include "VulkanSwapChain.h"

#include "Rendering/Glfw/GlfwWindow.h"

static const std::vector REQUIRED_DEVICE_EXTENSION {
	vk::KHRSwapchainExtensionName,
};

namespace Oyl::Rendering
{
	struct VulkanRenderContext::Impl
	{
		const IWindow* window;

		VulkanDeviceHandle device = nullptr;
		VulkanSwapChain swapChain = nullptr;

		void
		RecreateSwapChain();
	};

	VulkanRenderContext::VulkanRenderContext() noexcept
		: m_impl(nullptr) {}

	VulkanRenderContext::VulkanRenderContext(const CreateParams& a_params) noexcept
		: m_impl(nullptr)
	{
		Init(a_params);
	}

	VulkanRenderContext::VulkanRenderContext(VulkanRenderContext&& a_other) noexcept
		: m_impl(nullptr)
	{
		*this = std::move(a_other);
	}

	VulkanRenderContext&
	VulkanRenderContext::operator=(VulkanRenderContext&& a_other) noexcept
	{
		if (this != &a_other)
		{
			m_impl = std::move(a_other.m_impl);
		}
		return *this;
	}

	VulkanRenderContext::~VulkanRenderContext()
	{
		VulkanRenderContext::Destroy();
	}

	void
	VulkanRenderContext::Init(const CreateParams& a_params)
	{
		OYL_PROFILE_FUNCTION();

		if (!m_impl)
			m_impl = std::make_unique<Impl>();

		m_impl->window = a_params.window;

		m_impl->device = VulkanDevice::Create({
			.commandQueueFlags = CommandQueueFlagBits::Graphics | CommandQueueFlagBits::Transfer,
			.ppRequiredDeviceExtensionsData = REQUIRED_DEVICE_EXTENSION.data(),
			.requiredDeviceExtensionsLength = REQUIRED_DEVICE_EXTENSION.size(),
			.window = m_impl->window,
		});

		m_impl->swapChain = VulkanSwapChain(
			*m_impl->device,
			{
				.window = *m_impl->window,
			}
		);
	}

	void
	VulkanRenderContext::Update()
	{
		OYL_PROFILE_FUNCTION();

		if (!m_impl->window || !m_impl->window->IsValid())
			return;
	}

	void
	VulkanRenderContext::Destroy()
	{
		OYL_PROFILE_FUNCTION();

		if (!m_impl)
			return;

		m_impl->device->WaitUntilIdle();

		m_impl->swapChain.Destroy();
		m_impl->device->Destroy();

		*m_impl = {};
	}

	void
	VulkanRenderContext::Resize(Vector2u /*a_size*/)
	{
		// No need to pass in a_size - we get the size from the window directly
		m_impl->RecreateSwapChain();
	}

	const VulkanDevice*
	VulkanRenderContext::GetDevice() const
	{
		return m_impl->device.Get();
	}

	VulkanSwapChain*
	VulkanRenderContext::GetSwapChain()
	{
		return &m_impl->swapChain;
	}

	void
	VulkanRenderContext::Impl::RecreateSwapChain()
	{
		OYL_PROFILE_FUNCTION();

		swapChain.Recreate();
	}
}
