#include "VulkanRenderContext.h"

#include <vulkan/vulkan_raii.hpp>

#include "VulkanDevice.h"
#include "VulkanSwapChain.h"

#include "Rendering/Glfw/GlfwWindow.h"

static const std::vector REQUIRED_DEVICE_EXTENSION {
	vk::KHRSwapchainExtensionName,
};

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

namespace Oyl::Rendering::Vulkan
{
	struct RenderContext::Impl
	{
		const IWindow* window;

		Device device = nullptr;
		SwapChainImpl swapChain = nullptr;

		void
		RecreateSwapChain();
	};

	RenderContext::RenderContext() noexcept
		: m_impl(nullptr) {}

	RenderContext::RenderContext(const CreateParams& a_params) noexcept
		: m_impl(nullptr)
	{
		Init(a_params);
	}

	RenderContext::RenderContext(RenderContext&& a_other) noexcept
		: m_impl(nullptr)
	{
		*this = std::move(a_other);
	}

	RenderContext&
	RenderContext::operator=(RenderContext&& a_other) noexcept
	{
		if (this != &a_other)
		{
			m_impl = std::move(a_other.m_impl);
		}
		return *this;
	}

	RenderContext::~RenderContext()
	{
		RenderContext::Destroy();
	}

	void
	RenderContext::Init(const CreateParams& a_params)
	{
		OYL_PROFILE_FUNCTION();

		if (!m_impl)
			m_impl = std::make_unique<Impl>();

		m_impl->window = a_params.window;

		m_impl->device = DeviceImpl::Create(
			{
				.window = *m_impl->window,
				.commandQueueFlags = CommandQueueFlagBits::Graphics | CommandQueueFlagBits::Transfer,
				.ppRequiredDeviceExtensionsData = REQUIRED_DEVICE_EXTENSION.data(),
				.requiredDeviceExtensionsLength = REQUIRED_DEVICE_EXTENSION.size()
			}
		);

		m_impl->swapChain = SwapChainImpl(
			*m_impl->device,
			{
				.window = *m_impl->window,
			}
		);
	}

	void
	RenderContext::Update()
	{
		OYL_PROFILE_FUNCTION();

		if (!m_impl->window || !m_impl->window->IsValid())
			return;
	}

	void
	RenderContext::Destroy()
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
	RenderContext::Resize(Vector2u /*a_size*/)
	{
		// No need to pass in a_size - we get the size from the window directly
		m_impl->RecreateSwapChain();
	}

	const DeviceImpl*
	RenderContext::GetDevice() const
	{
		return m_impl->device.Get();
	}

	SwapChainImpl*
	RenderContext::GetSwapChain()
	{
		return &m_impl->swapChain;
	}

	void
	RenderContext::Impl::RecreateSwapChain()
	{
		OYL_PROFILE_FUNCTION();

		swapChain.Recreate();
	}
}
