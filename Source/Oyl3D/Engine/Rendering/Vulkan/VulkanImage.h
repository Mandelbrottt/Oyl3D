#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "Rendering/Image.h"

namespace Oyl::Rendering
{
	class VulkanDevice;
	class VulkanStagingBuffer;
	class VulkanCommandBuffer;

	using VulkanImageId = OpaqueHandleConvertible<ImageId, vk::Image>;

	class OYL_RENDERING_API VulkanImage : public Image
	{
		friend class VulkanCommandBuffer;

	public:
		VulkanImage(nullptr_t);

		VulkanImage(const VulkanDevice& a_device, const CreateParams& a_params);

		VulkanImage(VulkanImage&& a_other) noexcept;
		VulkanImage&
		operator =(VulkanImage&& a_other) noexcept;

		virtual
		~VulkanImage();

		void
		Destroy() override;

		bool
		IsValid() const override;

		Vector2u
		GetSize() const override;

		const VulkanStagingBuffer&
		GetStagingBuffer() const;

		ImageFormat
		GetFormat() const override;

		ImageUsageFlags
		GetUsageFlags() const override;

		ImageLayout
		GetLayout() const override;

	private:
		void
		SetLayout(ImageLayout a_layout);

	public:
		const vk::raii::Image&
		GetVkImage() const;

		vk::Format
		GetVkFormat() const;

		const vk::raii::DeviceMemory&
		GetVkDeviceMemory() const;

		const vk::raii::ImageView&
		GetVkImageView() const;

		VulkanImageId
		GetId() const;

		operator VulkanImageId() const
		{
			return GetId();
		}

	protected:
		ImageId
		GetIdImpl() const override
		{
			return GetId();
		}

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};

	using VulkanImageHandle = PImpl<VulkanImage>;
}
