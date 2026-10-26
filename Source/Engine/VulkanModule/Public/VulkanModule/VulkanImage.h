#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "VulkanRenderingEnums.h"
#include "VulkanStagingBuffer.h"

#include "Rendering/Image.h"

namespace Oyl
{
	class VulkanDevice;

	using VulkanImageId = IConvertibleDeviceObjectId<ImageId, vk::Image>;

	class VULKANMODULE_EXPORT VulkanImage : public IImage
	{
		friend class VulkanCommandBuffer;

	public:
		VulkanImage(nullptr_t) {}

		VulkanImage(const VulkanDevice& a_device, const ImageCreateParams& a_params);

		DEFAULT_MOVE(VulkanImage);

		virtual
		~VulkanImage();

		void
		Destroy() override;

		bool
		IsValid() const override;

		Vector2u
		GetSize() const override { return m_size; }

		const VulkanStagingBuffer&
		GetStagingBuffer() const { return m_stagingBuffer; }

		ImageFormat
		GetFormat() const override { return m_format; }

		ImageUsageFlags
		GetUsageFlags() const override { return m_usageFlags; }

		ImageLayout
		GetLayout() const override { return m_layout; }

		const vk::raii::Image&
		GetVkImage() const { return m_vkImage; }

		vk::Format
		GetVkFormat() const { return ToVkEnum(m_format); }

		const vk::raii::DeviceMemory&
		GetVkDeviceMemory() const { return m_vkImageMemory; }

		const vk::raii::ImageView&
		GetVkImageView() const { return m_vkImageView; }

		VulkanImageId
		GetId() const { return *m_vkImage; }

		operator VulkanImageId() const { return GetId(); }

	protected:
		ImageId
		GetIdImpl() const override { return GetId(); }

	private:
		void
		SetLayout(ImageLayout a_layout) { m_layout = a_layout; }

		void
		CreateImage(const VulkanDevice& a_device, const ImageCreateParams& a_params);

		void
		CreateImageView(const VulkanDevice& a_device, const ImageCreateParams& a_params);

		void
		CreateStagingBuffer(const VulkanDevice& a_device, const ImageCreateParams& a_params);

		void
		CopyStagingBufferToImage(VulkanImage& a_image, const VulkanDevice& a_device, const ImageCreateParams& a_params);

	private:
		VulkanStagingBuffer m_stagingBuffer = nullptr;

		vk::raii::Image m_vkImage = nullptr;
		vk::raii::DeviceMemory m_vkImageMemory = nullptr;
		vk::raii::ImageView m_vkImageView = nullptr;

		ImageFormat m_format = {};
		ImageUsageFlags m_usageFlags = {};
		ImageLayout m_layout = {};
		Vector2u m_size;
	};
}
