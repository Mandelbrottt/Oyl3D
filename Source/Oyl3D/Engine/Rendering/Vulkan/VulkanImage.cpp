#include "VulkanImage.h"

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
		CreateImage(const CreateParams& a_params);
		void
		CreateImageView(const CreateParams& a_params);

		void
		CreateStagingBuffer(const CreateParams& a_params);
		void
		CopyStagingBufferToImage(const CreateParams& a_params);
	};

	Image::Image()
		: m_impl(nullptr) {}

	Image::Image(const CreateParams& a_params)
		: m_impl(std::make_unique<Impl>())
	{
		OYL_PROFILE_FUNCTION();

		m_impl->CreateImage(a_params);
		m_impl->CreateImageView(a_params);

		if (a_params.pixelData != nullptr && a_params.pixelLength != 0)
		{
			m_impl->CreateStagingBuffer(a_params);
			m_impl->CopyStagingBufferToImage(a_params);
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
	Image::Impl::CreateImage(const CreateParams& a_params)
	{
		auto imageCreateInfo = vk::ImageCreateInfo {
			.imageType = vk::ImageType::e2D,
			.format = a_params.vkFormat,
			.extent = { .width = a_params.size.x, .height = a_params.size.y, .depth = 1 },
			.mipLevels = 1,
			.arrayLayers = 1,
			.samples = vk::SampleCountFlagBits::e1,
			.tiling = vk::ImageTiling::eOptimal,
			.usage = a_params.vkUsage,
			.sharingMode = vk::SharingMode::eExclusive,
			.initialLayout = vk::ImageLayout::eUndefined
		};
		vkImage = vk::raii::Image(a_params.device.GetVkDevice(), imageCreateInfo);

		auto memRequirements = vkImage.getMemoryRequirements();
		auto allocInfo = vk::MemoryAllocateInfo {
			.allocationSize = memRequirements.size,
			.memoryTypeIndex = FindMemoryType(
				a_params.device.GetVkPhysicalDevice(),
				memRequirements.memoryTypeBits,
				a_params.vkProperties
			)
		};
		vkImageMemory = vk::raii::DeviceMemory(a_params.device.GetVkDevice(), allocInfo);
		vkImage.bindMemory(vkImageMemory, 0);
	}

	void
	Image::Impl::CreateImageView(const CreateParams& a_params)
	{
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
		vkImageView = vk::raii::ImageView(a_params.device.GetVkDevice(), imageViewCreateInfo);
	}

	void
	Image::Impl::CreateStagingBuffer(const CreateParams& a_params)
	{
		OYL_ASSERT(a_params.pixelData && a_params.pixelLength != 0);

		stagingBuffer = StagingBuffer(
			{
				.device = a_params.device,
				.size = a_params.pixelLength,
				.vkUsage = vk::BufferUsageFlagBits::eTransferSrc,
				.vkProperties = vk::MemoryPropertyFlagBits::eHostVisible
				                | vk::MemoryPropertyFlagBits::eHostCoherent
			}
		);
		stagingBuffer.CopyMemory(a_params.pixelData, a_params.pixelLength);
	}

	void
	Image::Impl::CopyStagingBufferToImage(const CreateParams& a_params)
	{
		OYL_ASSERT(!!stagingBuffer);
	}
}
