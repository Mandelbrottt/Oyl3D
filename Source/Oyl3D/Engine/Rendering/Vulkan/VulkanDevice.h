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

namespace Oyl
{
	class IWindow;
}

namespace Oyl::Rendering
{
	class VulkanCommandQueue;

	class OYL_RENDERING_API VulkanDevice : public Device
	{
		struct DeviceTag {};

	public:
		VulkanDevice(nullptr_t);

		struct CreateParams
		{
			CommandQueueFlags commandQueueFlags;

			const char* const* ppRequiredDeviceExtensionsData;
			size_t requiredDeviceExtensionsLength;

			const IWindow* window;
		};

		static
		UniquePtrImplicitConvertible<VulkanDevice>
		Create(const CreateParams& a_params);

		explicit
		VulkanDevice(DeviceTag, const CreateParams& a_params);

		virtual
		~VulkanDevice();

		void
		Destroy() override;

		bool
		IsValid() const override;

		const VulkanCommandQueue*
		GetCommandQueue(CommandQueueFlagBits a_flag) const override;

		const vk::raii::Device&
		GetVkDevice() const;

		const vk::raii::PhysicalDevice&
		GetVkPhysicalDevice() const;

		const vk::raii::SurfaceKHR&
		GetVkSurface() const;

		void
		WaitUntilIdle() const override;

		CommandBufferHandle
		CreateCommandBuffer(const VulkanCommandBuffer::CreateParams& a_params) const override;

		CommandPoolHandle
		CreateCommandPool(const VulkanCommandPool::CreateParams& a_params) const override;

		ImageHandle
		CreateImage(const VulkanImage::CreateParams& a_params) const override;

		ShaderHandle
		CreateShader(const VulkanShader::CreateParams& a_params) const override;

		VertexBufferHandle
		CreateVertexBuffer(const VulkanVertexBuffer::CreateParams& a_params) const override;

		SemaphoreHandle
		CreateSemaphore() const override;

		FenceHandle
		CreateFence() const override;

	private:
		void
		CreateSurface();

		void
		PickPhysicalDevice(CommandQueueFlags a_queueFlags);

		void
		CreateLogicalDevice();

		void
		CreateCommandQueues();

	private:
		const IWindow* m_window = nullptr;

		std::vector<std::string> m_requiredDeviceExtensions;

		vk::raii::SurfaceKHR m_surface = nullptr;

		vk::raii::PhysicalDevice m_physicalDevice = nullptr;
		vk::raii::Device m_device = nullptr;

		std::unordered_map<CommandQueueFlagBits, uint32> m_queueFamilyIndices;
		std::unordered_map<CommandQueueFlagBits, VulkanCommandQueue> m_queues;
	};

	using VulkanDeviceHandle = UniquePtrImplicitConvertible<VulkanDevice>;
}
