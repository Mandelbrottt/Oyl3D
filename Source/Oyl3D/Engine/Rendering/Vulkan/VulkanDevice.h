#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "VulkanCommandBuffer.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandQueue.h"
#include "VulkanFence.h"
#include "VulkanImage.h"
#include "VulkanSemaphore.h"
#include "VulkanShader.h"
#include "VulkanVertexBuffer.h"

#include "Rendering/Device.h"

namespace Oyl::Glfw
{
	class Window;
}

namespace Oyl::Rendering::Vulkan
{
	class CommandQueueImpl;

	class OYL_RENDERING_API DeviceImpl : public Rendering::DeviceImpl
	{
		struct DeviceTag {};

	public:
		DeviceImpl(nullptr_t);

		struct CreateParams
		{
			const IWindow& window;

			CommandQueueFlags commandQueueFlags;

			const char* const* ppRequiredDeviceExtensionsData;
			size_t requiredDeviceExtensionsLength;
		};

		static
		PImpl<DeviceImpl>
		Create(const CreateParams& a_params);

		explicit
		DeviceImpl(DeviceTag, const CreateParams& a_params);

		virtual
		~DeviceImpl();

		void
		Destroy() override;

		bool
		IsValid() const override;

		const IWindow*
		GetWindow() const override;

		const CommandQueueImpl*
		GetCommandQueue(CommandQueueFlagBits a_flag) const override;

		const vk::raii::Device&
		GetVkDevice() const;

		const vk::raii::PhysicalDevice&
		GetVkPhysicalDevice() const;

		const vk::raii::SurfaceKHR&
		GetVkSurface() const;

		void
		WaitUntilIdle() const override;

		Rendering::CommandBuffer
		CreateCommandBuffer(const CommandBufferImpl::CreateParams& a_params) const override;

		Rendering::CommandPool
		CreateCommandPool(const CommandPoolImpl::CreateParams& a_params) const override;

		Rendering::Image
		CreateImage(const ImageImpl::CreateParams& a_params) const override;

		Rendering::Shader
		CreateShader(const ShaderImpl::CreateParams& a_params) const override;

		Rendering::VertexBuffer
		CreateVertexBuffer(const VertexBufferImpl::CreateParams& a_params) const override;

		Rendering::Semaphore
		CreateSemaphore() const override;

		Rendering::Fence
		CreateFence() const override;

	private:
		void
		CreateInstance();

		void
		CreateDebugMessenger();

		void
		CreateSurface();

		void
		PickPhysicalDevice(CommandQueueFlags a_queueFlags);

		void
		CreateLogicalDevice();

		void
		CreateCommandQueues();

	private:
		const Glfw::Window* m_window = nullptr;

		std::vector<std::string> m_requiredDeviceExtensions;

		vk::raii::Context m_context;
		vk::raii::Instance m_instance = nullptr;
		vk::raii::DebugUtilsMessengerEXT m_debugMessenger = nullptr;

		vk::raii::SurfaceKHR m_surface = nullptr;

		vk::raii::PhysicalDevice m_physicalDevice = nullptr;
		vk::raii::Device m_device = nullptr;

		std::unordered_map<CommandQueueFlagBits, uint32> m_queueFamilyIndices;
		std::unordered_map<CommandQueueFlagBits, CommandQueueImpl> m_queues;
	};

	using Device = PImpl<DeviceImpl>;
}
