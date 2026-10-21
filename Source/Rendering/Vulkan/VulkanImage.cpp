#include "VulkanImage.h"

#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanEnums.h"
#include "VulkanStagingBuffer.h"

namespace Oyl::Rendering
{
	struct VulkanImage::Impl
	{
		VulkanStagingBuffer stagingBuffer;

		vk::raii::Image vkImage = nullptr;
		vk::raii::DeviceMemory vkImageMemory = nullptr;
		vk::raii::ImageView vkImageView = nullptr;

		ImageFormat format;
		ImageUsageFlags usageFlags;
		ImageLayout layout;
		Vector2u size;

		void
		CreateImage(const VulkanDevice& a_device, const CreateParams& a_params);
		void
		CreateImageView(const VulkanDevice& a_device, const CreateParams& a_params);

		void
		CreateStagingBuffer(const VulkanDevice& a_device, const CreateParams& a_params);
		void
		CopyStagingBufferToImage(VulkanImage& a_image, const VulkanDevice& a_device, const CreateParams& a_params);
	};

	VulkanImage::VulkanImage(nullptr_t)
		: m_impl(nullptr) {}

	VulkanImage::VulkanImage(const VulkanDevice& a_device, const CreateParams& a_params)
		: m_impl(std::make_unique<Impl>())
	{
		OYL_PROFILE_FUNCTION();

		m_impl->CreateImage(a_device, a_params);
		m_impl->CreateImageView(a_device, a_params);

		if (!a_params.pixelData.Empty())
		{
			m_impl->CreateStagingBuffer(a_device, a_params);
			m_impl->CopyStagingBufferToImage(*this, a_device, a_params);
		}
	}

	VulkanImage::VulkanImage(VulkanImage&& a_other) noexcept
	{
		*this = std::move(a_other);
	}

	VulkanImage&
	VulkanImage::operator=(VulkanImage&& a_other) noexcept
	{
		if (this != &a_other)
		{
			std::swap(m_impl, a_other.m_impl);
		}
		return *this;
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

		m_impl->vkImage.clear();
	}

	bool
	VulkanImage::IsValid() const
	{
		return m_impl
		       && *m_impl->vkImage
		       && *m_impl->vkImageMemory
		       && *m_impl->vkImageView;
	}

	Vector2u
	VulkanImage::GetSize() const
	{
		return m_impl->size;
	}

	const VulkanStagingBuffer&
	VulkanImage::GetStagingBuffer() const
	{
		return m_impl->stagingBuffer;
	}

	ImageFormat
	VulkanImage::GetFormat() const
	{
		return m_impl->format;
	}

	ImageUsageFlags
	VulkanImage::GetUsageFlags() const
	{
		return m_impl->usageFlags;
	}

	ImageLayout
	VulkanImage::GetLayout() const
	{
		return m_impl->layout;
	}

	void
	VulkanImage::SetLayout(ImageLayout a_layout)
	{
		m_impl->layout = a_layout;
	}

	const vk::raii::Image&
	VulkanImage::GetVkImage() const
	{
		return m_impl->vkImage;
	}

	vk::Format
	VulkanImage::GetVkFormat() const
	{
		return ToVkEnum(m_impl->format);
	}

	const vk::raii::DeviceMemory&
	VulkanImage::GetVkDeviceMemory() const
	{
		return m_impl->vkImageMemory;
	}

	const vk::raii::ImageView&
	VulkanImage::GetVkImageView() const
	{
		return m_impl->vkImageView;
	}

	VulkanImageId
	VulkanImage::GetId() const
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
	VulkanImage::Impl::CreateImage(const VulkanDevice& a_device, const CreateParams& a_params)
	{
		OYL_PROFILE_FUNCTION();

		format = a_params.format;
		usageFlags = a_params.usageFlags;
		layout = ImageLayout::None;
		if (!a_params.pixelData.Empty())
		{
			usageFlags |= ImageUsageFlagBits::TransferDst;
			layout = a_params.layout;
		}

		auto imageCreateInfo = vk::ImageCreateInfo {
			.imageType = vk::ImageType::e2D,
			.format = ToVkEnum(a_params.format),
			.extent = { .width = a_params.size.x, .height = a_params.size.y, .depth = 1 },
			.mipLevels = 1,
			.arrayLayers = 1,
			.samples = vk::SampleCountFlagBits::e1,
			.tiling = vk::ImageTiling::eOptimal,
			.usage = ToVkEnum(usageFlags),
			.sharingMode = vk::SharingMode::eExclusive,
			.initialLayout = ToVkEnum(layout)
		};
		size = a_params.size;
		vkImage = vk::raii::Image(a_device.GetVkDevice(), imageCreateInfo);

		auto memRequirements = vkImage.getMemoryRequirements();
		auto allocInfo = vk::MemoryAllocateInfo {
			.allocationSize = memRequirements.size,
			.memoryTypeIndex = FindMemoryType(
				a_device.GetVkPhysicalDevice(),
				memRequirements.memoryTypeBits,
				vk::MemoryPropertyFlagBits::eDeviceLocal
			)
		};
		vkImageMemory = vk::raii::DeviceMemory(a_device.GetVkDevice(), allocInfo);
		vkImage.bindMemory(vkImageMemory, 0);
	}

	void
	VulkanImage::Impl::CreateImageView(const VulkanDevice& a_device, const CreateParams& a_params)
	{
		OYL_PROFILE_FUNCTION();

		vk::ImageViewCreateInfo imageViewCreateInfo {
			.image = vkImage,
			.viewType = vk::ImageViewType::e2D,
			.format = ToVkEnum(a_params.format),
			.subresourceRange = {
				.aspectMask = vk::ImageAspectFlagBits::eColor,
				.levelCount = 1,
				.layerCount = 1
			},
		};
		vkImageView = vk::raii::ImageView(a_device.GetVkDevice(), imageViewCreateInfo);
	}

	void
	VulkanImage::Impl::CreateStagingBuffer(const VulkanDevice& a_device, const CreateParams& a_params)
	{
		OYL_PROFILE_FUNCTION();

		OYL_ASSERT(!a_params.pixelData.Empty());

		stagingBuffer = VulkanStagingBuffer(
			a_device,
			{
				.dataLength = a_params.pixelData.Length(),
				.vkUsage = vk::BufferUsageFlagBits::eTransferSrc,
				.vkProperties = vk::MemoryPropertyFlagBits::eHostVisible
				                | vk::MemoryPropertyFlagBits::eHostCoherent
			}
		);
		stagingBuffer.CopyMemory(a_params.pixelData.Data(), a_params.pixelData.Length());
	}

	void
	VulkanImage::Impl::CopyStagingBufferToImage(
		VulkanImage& a_image,
		const VulkanDevice& a_device,
		const CreateParams& a_params
	)
	{
		OYL_PROFILE_FUNCTION();

		OYL_ASSERT(!!stagingBuffer);

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
			.imageExtent = { size.x, size.y, 0 }
		};

		auto& vkCommandBuffer = commandBuffer.GetVkCommandBuffer();
		vkCommandBuffer.begin({ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit });

		commandBuffer.TransitionImageLayout(a_image, ImageLayout::TransferDest);
		vkCommandBuffer.copyBufferToImage(
			stagingBuffer.GetVkBuffer(),
			vkImage,
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
