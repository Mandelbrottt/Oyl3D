#include "VulkanImage.h"

#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanRenderingEnums.h"
#include "VulkanStagingBuffer.h"

namespace Oyl
{
	VulkanImage::VulkanImage(const VulkanDevice& a_device, const ImageCreateParams& a_params)
	{
		OYL_PROFILE_FUNCTION();

		CreateImage(a_device, a_params);
		CreateImageView(a_device, a_params);

		if (!a_params.pixelData.Empty())
		{
			CreateStagingBuffer(a_device, a_params);
			CopyStagingBufferToImage(*this, a_device, a_params);
		}
	}

	VulkanImage::~VulkanImage()
	{
		VulkanImage::Destroy();
	}

	void
	VulkanImage::Destroy()
	{
		if (!IsValid())
			return;

		m_vkImage.clear();
	}

	bool
	VulkanImage::IsValid() const
	{
		return *m_vkImage
		       && *m_vkImageMemory
		       && *m_vkImageView;
	}

	static
	uint32
	FindMemoryType(
		const vk::raii::PhysicalDevice& a_physicalDevice,
		uint32 a_typeFilter,
		vk::MemoryPropertyFlags a_properties
	)
	{
		auto memoryProperties = a_physicalDevice.getMemoryProperties();

		for (uint32 i = 0; i < memoryProperties.memoryTypeCount; i++)
		{
			bool isMemorySuitable =
				(a_typeFilter & (1 << i))
				&& (memoryProperties.memoryTypes[i].propertyFlags & a_properties) == a_properties;

			if (isMemorySuitable)
				return i;
		}

		throw std::runtime_error("Failed to find suitable memory type!");
	}

	void
	VulkanImage::CreateImage(const VulkanDevice& a_device, const ImageCreateParams& a_params)
	{
		OYL_PROFILE_FUNCTION();

		m_format = a_params.format;
		m_usageFlags = a_params.usageFlags;
		m_layout = ImageLayout::None;
		if (!a_params.pixelData.Empty())
		{
			m_usageFlags |= ImageUsageFlagBits::TransferDst;
			m_layout = a_params.layout;
		}

		auto imageCreateInfo = vk::ImageCreateInfo {
			.imageType = vk::ImageType::e2D,
			.format = ToVkEnum(a_params.format),
			.extent = { .width = a_params.size.x, .height = a_params.size.y, .depth = 1 },
			.mipLevels = 1,
			.arrayLayers = 1,
			.samples = vk::SampleCountFlagBits::e1,
			.tiling = vk::ImageTiling::eOptimal,
			.usage = ToVkEnum(m_usageFlags),
			.sharingMode = vk::SharingMode::eExclusive,
			.initialLayout = ToVkEnum(m_layout)
		};
		m_size = a_params.size;
		m_vkImage = vk::raii::Image(a_device.GetVkDevice(), imageCreateInfo);

		auto memRequirements = m_vkImage.getMemoryRequirements();
		auto allocInfo = vk::MemoryAllocateInfo {
			.allocationSize = memRequirements.size,
			.memoryTypeIndex = FindMemoryType(
				a_device.GetVkPhysicalDevice(),
				memRequirements.memoryTypeBits,
				vk::MemoryPropertyFlagBits::eDeviceLocal
			)
		};
		m_vkImageMemory = vk::raii::DeviceMemory(a_device.GetVkDevice(), allocInfo);
		m_vkImage.bindMemory(m_vkImageMemory, 0);
	}

	void
	VulkanImage::CreateImageView(const VulkanDevice& a_device, const ImageCreateParams& a_params)
	{
		OYL_PROFILE_FUNCTION();

		vk::ImageViewCreateInfo imageViewCreateInfo {
			.image = m_vkImage,
			.viewType = vk::ImageViewType::e2D,
			.format = ToVkEnum(a_params.format),
			.subresourceRange = {
				.aspectMask = vk::ImageAspectFlagBits::eColor,
				.levelCount = 1,
				.layerCount = 1
			},
		};
		m_vkImageView = vk::raii::ImageView(a_device.GetVkDevice(), imageViewCreateInfo);
	}

	void
	VulkanImage::CreateStagingBuffer(const VulkanDevice& a_device, const ImageCreateParams& a_params)
	{
		OYL_PROFILE_FUNCTION();

		OYL_ASSERT(!a_params.pixelData.Empty());

		m_stagingBuffer = VulkanStagingBuffer(
			a_device,
			{
				.dataLength = a_params.pixelData.Length(),
				.vkUsage = vk::BufferUsageFlagBits::eTransferSrc,
				.vkProperties = vk::MemoryPropertyFlagBits::eHostVisible
				                | vk::MemoryPropertyFlagBits::eHostCoherent
			}
		);
		m_stagingBuffer.CopyMemory(a_params.pixelData.Data(), a_params.pixelData.Length());
	}

	void
	VulkanImage::CopyStagingBufferToImage(
		VulkanImage& a_image,
		const VulkanDevice& a_device,
		const ImageCreateParams& a_params
	)
	{
		OYL_PROFILE_FUNCTION();

		OYL_ASSERT(m_stagingBuffer.IsValid());

		auto commandPool = VulkanCommandPool(a_device, { .commandQueueFlags = CommandQueueFlagBits::Transfer });
		auto commandBuffer = VulkanCommandBuffer(a_device, { .commandPool = commandPool });

		vk::BufferImageCopy region {
			.bufferOffset = 0,
			.bufferRowLength = 0,
			.bufferImageHeight = 0,
			.imageSubresource = {
				.aspectMask = vk::ImageAspectFlagBits::eColor,
				.mipLevel = 0,
				.baseArrayLayer = 0,
				.layerCount = 1
			},
			.imageOffset = { 0, 0, 0 },
			.imageExtent = { m_size.x, m_size.y, 0 }
		};

		auto& vkCommandBuffer = commandBuffer.GetVkCommandBuffer();
		vkCommandBuffer.begin({ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit });

		commandBuffer.TransitionImageLayout(a_image, ImageLayout::TransferDest);
		vkCommandBuffer.copyBufferToImage(
			m_stagingBuffer.GetVkBuffer(),
			m_vkImage,
			vk::ImageLayout::eTransferDstOptimal,
			region
		);
		commandBuffer.TransitionImageLayout(a_image, a_params.layout);

		commandBuffer.End();

		auto& commandQueue = *a_device.GetCommandQueue(CommandQueueFlagBits::Transfer);
		commandQueue.Submit({ .commandBuffer = commandBuffer });
		commandQueue.WaitUntilIdle();
	}
}
