#include "VulkanImage.h"

#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanStagingBuffer.h"

namespace Oyl::Rendering::Vulkan
{
	struct Image::Impl
	{
		StagingBuffer stagingBuffer;

		vk::raii::Image vkImage = nullptr;
		vk::raii::DeviceMemory vkImageMemory = nullptr;
		vk::raii::ImageView vkImageView = nullptr;

		Vector2u size;

		void
		CreateImage(const Device& a_device, const CreateParams& a_params);
		void
		CreateImageView(const Device& a_device, const CreateParams& a_params);

		void
		CreateStagingBuffer(const Device& a_device, const CreateParams& a_params);
		void
		CopyStagingBufferToImage(Image& a_image, const Device& a_device, const CreateParams& a_params);
	};

	Image::Image()
		: m_impl(nullptr) {}

	Image::Image(const Device& a_device, const CreateParams& a_params)
		: m_impl(std::make_unique<Impl>())
	{
		OYL_PROFILE_FUNCTION();

		m_impl->CreateImage(a_device, a_params);
		m_impl->CreateImageView(a_device, a_params);

		if (a_params.pixelData != nullptr && a_params.pixelLength != 0)
		{
			m_impl->CreateStagingBuffer(a_device, a_params);
			m_impl->CopyStagingBufferToImage(*this, a_device, a_params);
		}
	}

	Image::Image(Image&& a_other) noexcept
	{
		*this = std::move(a_other);
	}

	Image&
	Image::operator=(Image&& a_other) noexcept
	{
		if (this != &a_other)
		{
			std::swap(m_impl, a_other.m_impl);
		}
		return *this;
	}

	Image::~Image()
	{
		Image::Destroy();
	}

	void
	Image::Destroy()
	{
		if (!IsValid())
			return;

		m_impl->vkImage.clear();
	}

	bool
	Image::IsValid() const
	{
		return m_impl
		       && *m_impl->vkImage
		       && *m_impl->vkImageMemory
		       && *m_impl->vkImageView;
	}

	void
	Image::VkTransitionImageLayout(
		const CommandBuffer& a_commandBuffer,
		vk::ImageLayout a_oldLayout,
		vk::ImageLayout a_newLayout,
		vk::AccessFlags2 a_srcAccessMask,
		vk::AccessFlags2 a_dstAccessMask,
		vk::PipelineStageFlags2 a_srcStageMask,
		vk::PipelineStageFlags2 a_dstStageMask
	)
	{
		OYL_PROFILE_FUNCTION();

		vk::ImageMemoryBarrier2 barrier = {
			.srcStageMask = a_srcStageMask,
			.srcAccessMask = a_srcAccessMask,
			.dstStageMask = a_dstStageMask,
			.dstAccessMask = a_dstAccessMask,
			.oldLayout = a_oldLayout,
			.newLayout = a_newLayout,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = m_impl->vkImage,
			.subresourceRange = {
				.aspectMask = vk::ImageAspectFlagBits::eColor,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		};
		vk::DependencyInfo dependency_info = {
			.dependencyFlags = {},
			.imageMemoryBarrierCount = 1,
			.pImageMemoryBarriers = &barrier
		};
		const auto& vkCommandBuffer = a_commandBuffer.GetVkCommandBuffer();
		vkCommandBuffer.pipelineBarrier2(dependency_info);
	}

	Vector2u
	Image::GetSize() const
	{
		return m_impl->size;
	}

	const StagingBuffer&
	Image::GetStagingBuffer() const
	{
		return m_impl->stagingBuffer;
	}

	const vk::raii::Image&
	Image::GetVkImage() const
	{
		return m_impl->vkImage;
	}

	const vk::raii::DeviceMemory&
	Image::GetVkDeviceMemory() const
	{
		return m_impl->vkImageMemory;
	}

	const vk::raii::ImageView&
	Image::GetVkImageView() const
	{
		return m_impl->vkImageView;
	}

	ImageHandle
	Image::GetHandle() const
	{
		return *m_impl->vkImage;
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
	Image::Impl::CreateImage(const Device& a_device, const CreateParams& a_params)
	{
		OYL_PROFILE_FUNCTION();

		auto vkUsage = a_params.vkUsage;
		auto vkInitialLayout = vk::ImageLayout::eUndefined;
		if (a_params.pixelData && a_params.pixelLength != 0)
		{
			vkUsage |= vk::ImageUsageFlagBits::eTransferDst;
			vkInitialLayout = a_params.vkLayout;
		}

		auto imageCreateInfo = vk::ImageCreateInfo {
			.imageType = vk::ImageType::e2D,
			.format = a_params.vkFormat,
			.extent = { .width = a_params.size.x, .height = a_params.size.y, .depth = 1 },
			.mipLevels = 1,
			.arrayLayers = 1,
			.samples = vk::SampleCountFlagBits::e1,
			.tiling = vk::ImageTiling::eOptimal,
			.usage = vkUsage,
			.sharingMode = vk::SharingMode::eExclusive,
			.initialLayout = vkInitialLayout
		};
		vkImage = vk::raii::Image(a_device.GetVkDevice(), imageCreateInfo);
		size = a_params.size;

		auto memRequirements = vkImage.getMemoryRequirements();
		auto allocInfo = vk::MemoryAllocateInfo {
			.allocationSize = memRequirements.size,
			.memoryTypeIndex = FindMemoryType(
				a_device.GetVkPhysicalDevice(),
				memRequirements.memoryTypeBits,
				a_params.vkProperties
			)
		};
		vkImageMemory = vk::raii::DeviceMemory(a_device.GetVkDevice(), allocInfo);
		vkImage.bindMemory(vkImageMemory, 0);
	}

	void
	Image::Impl::CreateImageView(const Device& a_device, const CreateParams& a_params)
	{
		OYL_PROFILE_FUNCTION();

		vk::ImageViewCreateInfo imageViewCreateInfo {
			.image = vkImage,
			.viewType = vk::ImageViewType::e2D,
			.format = a_params.vkFormat,
			.subresourceRange = {
				.aspectMask = vk::ImageAspectFlagBits::eColor,
				.levelCount = 1,
				.layerCount = 1
			},
		};
		vkImageView = vk::raii::ImageView(a_device.GetVkDevice(), imageViewCreateInfo);
	}

	void
	Image::Impl::CreateStagingBuffer(const Device& a_device, const CreateParams& a_params)
	{
		OYL_PROFILE_FUNCTION();

		OYL_ASSERT(a_params.pixelData && a_params.pixelLength != 0);

		stagingBuffer = StagingBuffer(
			a_device,
			{
				.dataLength = a_params.pixelLength,
				.vkUsage = vk::BufferUsageFlagBits::eTransferSrc,
				.vkProperties = vk::MemoryPropertyFlagBits::eHostVisible
				                | vk::MemoryPropertyFlagBits::eHostCoherent
			}
		);
		stagingBuffer.CopyMemory(a_params.pixelData, a_params.pixelLength);
	}

	void
	Image::Impl::CopyStagingBufferToImage(Image& a_image, const Device& a_device, const CreateParams& a_params)
	{
		OYL_PROFILE_FUNCTION();

		OYL_ASSERT(!!stagingBuffer);

		auto commandPool = CommandPool(a_device, { .commandQueueFlags = CommandQueueFlagBits::Transfer });
		auto commandBuffer = CommandBuffer(a_device, { .commandPool = commandPool });

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
			.imageExtent = { size.x, size.y, 0 }
		};

		auto& vkCommandBuffer = commandBuffer.GetVkCommandBuffer();
		vkCommandBuffer.begin({ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit });

		a_image.VkTransitionImageLayout(
			commandBuffer,
			vk::ImageLayout::eUndefined,
			vk::ImageLayout::eTransferDstOptimal,
			{},
			vk::AccessFlagBits2::eTransferWrite,
			vk::PipelineStageFlagBits2::eTopOfPipe,
			vk::PipelineStageFlagBits2::eTransfer
		);

		vkCommandBuffer.copyBufferToImage(
			stagingBuffer.GetVkBuffer(),
			vkImage,
			vk::ImageLayout::eTransferDstOptimal,
			region
		);

		a_image.VkTransitionImageLayout(
			commandBuffer,
			vk::ImageLayout::eTransferDstOptimal,
			a_params.vkLayout,
			vk::AccessFlagBits2::eTransferWrite,
			vk::AccessFlagBits2::eShaderRead | vk::AccessFlagBits2::eColorAttachmentWrite,
			vk::PipelineStageFlagBits2::eTransfer,
			vk::PipelineStageFlagBits2::eFragmentShader | vk::PipelineStageFlagBits2::eColorAttachmentOutput
		);

		vkCommandBuffer.end();

		auto& commandQueue = *a_device.GetCommandQueue(CommandQueueFlagBits::Transfer);
		commandQueue.Submit({ .commandBuffer = commandBuffer });
		commandQueue.GetVkQueue().waitIdle();
	}
}
