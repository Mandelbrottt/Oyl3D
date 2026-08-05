#include "VulkanDevice.h"

#include <vulkan/vulkan_raii.hpp>

#include <GLFW/glfw3.h>

#include "VulkanCommandBuffer.h"
#include "VulkanEnums.h"
#include "VulkanRenderEngine.h"

#include "Rendering/Glfw/GlfwWindow.h"

namespace Oyl::Rendering
{
	VulkanDevice::VulkanDevice(nullptr_t) {}

	VulkanDeviceHandle
	VulkanDevice::Create(const CreateParams& a_params)
	{
		return VulkanDeviceHandle(std::make_unique<VulkanDevice>(DeviceTag(), a_params));
	}

	VulkanDevice::VulkanDevice(DeviceTag, const CreateParams& a_params)
	{
		OYL_PROFILE_FUNCTION();

		m_window = static_cast<const Glfw::Window*>(a_params.window);

		if (a_params.ppRequiredDeviceExtensionsData && a_params.requiredDeviceExtensionsLength > 0)
		{
			m_requiredDeviceExtensions.reserve(a_params.requiredDeviceExtensionsLength);
			m_requiredDeviceExtensions.insert(
				m_requiredDeviceExtensions.end(),
				a_params.ppRequiredDeviceExtensionsData,
				a_params.ppRequiredDeviceExtensionsData + a_params.requiredDeviceExtensionsLength
			);
		}

		for (uint32 i = 0; i < sizeof(a_params.commandQueueFlags) * 8; i++)
		{
			auto flag = CommandQueueFlagBits(1 << i);
			if (a_params.commandQueueFlags & flag)
			{
				m_queueFamilyIndices[flag] = 0;
			}
		}

		CreateSurface();
		PickPhysicalDevice(a_params.commandQueueFlags);
		CreateLogicalDevice();
		CreateCommandQueues();
	}

	VulkanDevice::~VulkanDevice()
	{
		VulkanDevice::Destroy();
	}

	void
	VulkanDevice::Destroy()
	{
		OYL_PROFILE_FUNCTION();

		if (!IsValid())
			return;

		m_queues.clear();
		m_queueFamilyIndices.clear();
		m_device = nullptr;
		m_physicalDevice = nullptr;
		m_requiredDeviceExtensions.clear();
	}

	bool
	VulkanDevice::IsValid() const
	{
		return *m_device;
	}

	const VulkanCommandQueue*
	VulkanDevice::GetCommandQueue(CommandQueueFlagBits a_flag) const
	{
		auto iter = m_queues.find(a_flag);
		if (iter == m_queues.end())
			return nullptr;

		return &iter->second;
	}

	const vk::raii::Device&
	VulkanDevice::GetVkDevice() const
	{
		return m_device;
	}

	const vk::raii::PhysicalDevice&
	VulkanDevice::GetVkPhysicalDevice() const
	{
		return m_physicalDevice;
	}

	const vk::raii::SurfaceKHR&
	VulkanDevice::GetVkSurface() const
	{
		return m_surface;
	}

	void
	VulkanDevice::WaitUntilIdle() const
	{
		OYL_PROFILE_FUNCTION();

		m_device.waitIdle();
	}

	Rendering::CommandBufferHandle
	VulkanDevice::CreateCommandBuffer(const VulkanCommandBuffer::CreateParams& a_params) const
	{
		return VulkanCommandBufferHandle(std::make_unique<VulkanCommandBuffer>(*this, a_params));
	}

	Rendering::CommandPoolHandle
	VulkanDevice::CreateCommandPool(const VulkanCommandPool::CreateParams& a_params) const
	{
		return VulkanCommandPoolHandle(std::make_unique<VulkanCommandPool>(*this, a_params));
	}

	Rendering::ImageHandle
	VulkanDevice::CreateImage(const VulkanImage::CreateParams& a_params) const
	{
		return VulkanImageHandle(std::make_unique<VulkanImage>(*this, a_params));
	}

	Rendering::ShaderHandle
	VulkanDevice::CreateShader(const VulkanShader::CreateParams& a_params) const
	{
		return VulkanShaderHandle(std::make_unique<VulkanShader>(*this, a_params));
	}

	Rendering::VertexBufferHandle
	VulkanDevice::CreateVertexBuffer(const Rendering::VertexBuffer::CreateParams& a_params) const
	{
		return VulkanVertexBufferHandle(std::make_unique<VulkanVertexBuffer>(*this, a_params));
	}

	Rendering::SemaphoreHandle
	VulkanDevice::CreateSemaphore() const
	{
		return VulkanSemaphoreHandle(std::make_unique<VulkanSemaphore>(*this));
	}

	Rendering::FenceHandle
	VulkanDevice::CreateFence() const
	{
		return VulkanFenceHandle(std::make_unique<VulkanFence>(*this));
	}

	void
	VulkanDevice::CreateSurface()
	{
		OYL_PROFILE_FUNCTION();

		auto& instance = VulkanRenderEngine::GetVkInstance();

		VkSurfaceKHR cSurface;
		auto glfwWindow = static_cast<GLFWwindow*>(m_window->GetNativeWindowHandle());
		if (glfwCreateWindowSurface(*instance, glfwWindow, nullptr, &cSurface) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create window surface!");
		}
		m_surface = vk::raii::SurfaceKHR(instance, cSurface);
	}

	bool
	IsDeviceSuitable(
		const vk::raii::PhysicalDevice& a_physicalDevice,
		vk::QueueFlags a_queueFlags,
		const std::vector<std::string>& a_requiredDeviceExtensions
	)
	{
		// Check if the physicalDevice supports the Vulkan 1.3 API version
		bool supportsVulkan1_3 = a_physicalDevice.getProperties().apiVersion >= vk::ApiVersion13;

		// Check if any of the queue families support requested operations
		auto queueFamilies = a_physicalDevice.getQueueFamilyProperties();
		bool supportsGraphics = std::ranges::any_of(
			queueFamilies,
			[&](const vk::QueueFamilyProperties& a_qfp)
			{
				auto mask = a_qfp.queueFlags & a_queueFlags;
				return mask == a_queueFlags;
			}
		);

		// Check if all required physicalDevice extensions are available
		auto availableDeviceExtensions = a_physicalDevice.enumerateDeviceExtensionProperties();
		bool supportsAllRequiredExtensions =
			std::ranges::all_of(
				a_requiredDeviceExtensions,
				[&availableDeviceExtensions](const std::string& a_requiredDeviceExtension)
				{
					return std::ranges::any_of(
						availableDeviceExtensions,
						[a_requiredDeviceExtension](const vk::ExtensionProperties& a_availableDeviceExtension)
						{
							auto compare = strcmp(
								a_availableDeviceExtension.extensionName,
								a_requiredDeviceExtension.c_str()
							);
							return compare == 0;
						}
					);
				}
			);

		// Check if the physicalDevice supports the required features (shader draw parameters, dynamic rendering and extended dynamic state)
		auto features = a_physicalDevice.getFeatures2<vk::PhysicalDeviceFeatures2,
		                                              vk::PhysicalDeviceVulkan11Features,
		                                              vk::PhysicalDeviceVulkan13Features,
		                                              vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
		bool supportsRequiredFeatures = features.get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters &&
		                                features.get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering &&
		                                features.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState;

		// Return true if the physicalDevice meets all the criteria
		return supportsVulkan1_3 && supportsGraphics && supportsAllRequiredExtensions && supportsRequiredFeatures;
	}

	void
	VulkanDevice::PickPhysicalDevice(CommandQueueFlags a_queueFlags)
	{
		OYL_PROFILE_FUNCTION();

		auto& instance = VulkanRenderEngine::GetVkInstance();

		auto physicalDevices = instance.enumeratePhysicalDevices();
		const auto iter = std::ranges::find_if(
			physicalDevices,
			[&](const vk::raii::PhysicalDevice& a_physicalDevice)
			{
				auto vkQueueFlags = ToVkEnum(a_queueFlags);
				return IsDeviceSuitable(a_physicalDevice, vkQueueFlags, m_requiredDeviceExtensions);
			}
		);
		if (iter == physicalDevices.end())
		{
			throw std::runtime_error("failed to find a suitable GPU with Vulkan support!");
		}
		m_physicalDevice = *iter;
	}

	void
	VulkanDevice::CreateLogicalDevice()
	{
		OYL_PROFILE_FUNCTION();

		std::vector<vk::QueueFamilyProperties> queueFamilyProperties = m_physicalDevice.getQueueFamilyProperties();

		for (auto& [requestedFlag, index] : m_queueFamilyIndices)
		{
			// get the first index into queueFamilyProperties which supports the requested operations
			index = ~0u;
			for (uint32 propertiesIndex = 0; propertiesIndex < queueFamilyProperties.size(); propertiesIndex++)
			{
				auto queueFlags = queueFamilyProperties[propertiesIndex].queueFlags;
				vk::QueueFlags vkRequestedFlag = ToVkEnum(requestedFlag);

				if (queueFlags & vk::QueueFlagBits::eGraphics)
				{
					// If the queue supports graphics, we also want it to support SurfaceKHR
					if (!m_physicalDevice.getSurfaceSupportKHR(propertiesIndex, *m_surface))
						continue;

					// We want a unique queue for transfers
					if (requestedFlag == CommandQueueFlagBits::Transfer)
						continue;
				}

				if (queueFlags & vkRequestedFlag)
				{
					index = propertiesIndex;
					break;
				}
			}
			if (index == ~0u)
			{
				throw std::runtime_error("Could not find a queue for requested operations -> terminating");
			}
		}

		// Create a chain of feature structures
		vk::StructureChain featureChain = {
			vk::PhysicalDeviceFeatures2(),
			vk::PhysicalDeviceVulkan11Features { .shaderDrawParameters = true },
			vk::PhysicalDeviceVulkan13Features { .synchronization2 = true, .dynamicRendering = true },
			vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT { .extendedDynamicState = true }
		};

		std::vector<vk::DeviceQueueCreateInfo> deviceQueueCreateInfo;
		float queuePriority = 0.5f;
		for (const auto& [flag, index] : m_queueFamilyIndices)
			deviceQueueCreateInfo.emplace_back(
				vk::DeviceQueueCreateInfo {
					.queueFamilyIndex = index,
					.queueCount = 1,
					.pQueuePriorities = &queuePriority,
				}
			);

		// Vulkan needs the extensions array as c-strings
		std::vector<const char*> requiredExtensions(m_requiredDeviceExtensions.size(), nullptr);
		for (size_t i = 0; i < m_requiredDeviceExtensions.size(); i++)
		{
			requiredExtensions[i] = m_requiredDeviceExtensions[i].c_str();
		}

		vk::DeviceCreateInfo deviceCreateInfo {
			.pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
			.queueCreateInfoCount = (uint32) deviceQueueCreateInfo.size(),
			.pQueueCreateInfos = deviceQueueCreateInfo.data(),
			.enabledExtensionCount = (uint32) m_requiredDeviceExtensions.size(),
			.ppEnabledExtensionNames = requiredExtensions.data(),
		};

		m_device = vk::raii::Device(m_physicalDevice, deviceCreateInfo);
	}

	void
	VulkanDevice::CreateCommandQueues()
	{
		for (const auto& [flag, index] : m_queueFamilyIndices)
		{
			m_queues.emplace(
				flag,
				VulkanCommandQueue(
					*this,
					{
						.queueFamilyIndex = index
					}
				)
			);
		}
	}
}
