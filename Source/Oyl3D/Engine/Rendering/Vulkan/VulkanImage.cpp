#include "VulkanImage.h"

#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanEnums.h"
#include "VulkanStagingBuffer.h"

namespace Oyl::Rendering::Vulkan
{
	struct ImageImpl::Impl
	{
		StagingBuffer stagingBuffer;

		vk::raii::Image vkImage = nullptr;
		vk::raii::DeviceMemory vkImageMemory = nullptr;
		vk::raii::ImageView vkImageView = nullptr;

		ImageFormat format;
		ImageUsageFlags usageFlags;
		ImageLayout layout;
		Vector2u size;

		void
		CreateImage(const DeviceImpl& a_device, const CreateParams& a_params);
		void
		CreateImageView(const DeviceImpl& a_device, const CreateParams& a_params);

		void
		CreateStagingBuffer(const DeviceImpl& a_device, const CreateParams& a_params);
		void
		CopyStagingBufferToImage(ImageImpl& a_image, const DeviceImpl& a_device, const CreateParams& a_params);
	};

	ImageImpl::ImageImpl(nullptr_t)
		: m_impl(nullptr) {}

	ImageImpl::ImageImpl(const DeviceImpl& a_device, const CreateParams& a_params)
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

	ImageImpl::ImageImpl(ImageImpl&& a_other) noexcept
	{
		*this = std::move(a_other);
	}

	ImageImpl&
	ImageImpl::operator=(ImageImpl&& a_other) noexcept
	{
		if (this != &a_other)
		{
			std::swap(m_impl, a_other.m_impl);
		}
		return *this;
	}

	ImageImpl::~ImageImpl()
	{
		ImageImpl::Destroy();
	}

	void
	ImageImpl::Destroy()
	{
		if (!IsValid())
			return;

		m_impl->vkImage.clear();
	}

	bool
	ImageImpl::IsValid() const
	{
		return m_impl
		       && *m_impl->vkImage
		       && *m_impl->vkImageMemory
		       && *m_impl->vkImageView;
	}

	Vector2u
	ImageImpl::GetSize() const
	{
		return m_impl->size;
	}

	const StagingBuffer&
	ImageImpl::GetStagingBuffer() const
	{
		return m_impl->stagingBuffer;
	}

	ImageFormat
	ImageImpl::GetFormat() const
	{
		return m_impl->format;
	}

	ImageUsageFlags
	ImageImpl::GetUsageFlags() const
	{
		return m_impl->usageFlags;
	}

	ImageLayout
	ImageImpl::GetLayout() const
	{
		return m_impl->layout;
	}

	void
	ImageImpl::SetLayout(ImageLayout a_layout)
	{
		m_impl->layout = a_layout;
	}

	const vk::raii::Image&
	ImageImpl::GetVkImage() const
	{
		return m_impl->vkImage;
	}

	vk::Format
	ImageImpl::GetVkFormat() const
	{
		return ToVkEnum(m_impl->format);
	}

	const vk::raii::DeviceMemory&
	ImageImpl::GetVkDeviceMemory() const
	{
		return m_impl->vkImageMemory;
	}

	const vk::raii::ImageView&
	ImageImpl::GetVkImageView() const
	{
		return m_impl->vkImageView;
	}

	ImageHandle
	ImageImpl::GetHandle() const
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
	ImageImpl::Impl::CreateImage(const DeviceImpl& a_device, const CreateParams& a_params)
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
	ImageImpl::Impl::CreateImageView(const DeviceImpl& a_device, const CreateParams& a_params)
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
	ImageImpl::Impl::CreateStagingBuffer(const DeviceImpl& a_device, const CreateParams& a_params)
	{
		OYL_PROFILE_FUNCTION();

		OYL_ASSERT(!a_params.pixelData.Empty());

		stagingBuffer = StagingBuffer(
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
	ImageImpl::Impl::CopyStagingBufferToImage(
		ImageImpl& a_image,
		const DeviceImpl& a_device,
		const CreateParams& a_params
	)
	{
		OYL_PROFILE_FUNCTION();

		OYL_ASSERT(!!stagingBuffer);

		auto commandPool = CommandPoolImpl(a_device, { .commandQueueFlags = CommandQueueFlagBits::Transfer });
		auto commandBuffer = CommandBufferImpl(a_device, { .commandPool = commandPool });

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
