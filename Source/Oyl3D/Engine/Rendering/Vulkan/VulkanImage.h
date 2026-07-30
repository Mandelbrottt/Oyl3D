#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "Rendering/Image.h"

namespace Oyl::Rendering::Vulkan
{
	class DeviceImpl;
	class StagingBuffer;
	class CommandBuffer;

	using ImageHandle = OpaqueHandleConvertible<ImageHandle, vk::Image>;

	class OYL_RENDERING_API Image : public Rendering::Image
	{
	public:
		Image();

		struct CreateParams
		{
			const byte* pixelData = nullptr;
			uint32 pixelLength = 0;

			Vector2u size;

			vk::Format vkFormat;

			vk::ImageUsageFlags vkUsage;
			vk::MemoryPropertyFlags vkProperties;
			vk::ImageLayout vkLayout = vk::ImageLayout::eUndefined;
		};

		explicit
		Image(const DeviceImpl& a_device, const CreateParams& a_params);

		Image(Image&& a_other) noexcept;
		Image&
		operator =(Image&& a_other) noexcept;

		virtual
		~Image();

		void
		Destroy() override;

		bool
		IsValid() const override;

		void
		VkTransitionImageLayout(
			const CommandBuffer& a_commandBuffer,
			vk::ImageLayout a_oldLayout,
			vk::ImageLayout a_newLayout,
			vk::AccessFlags2 a_srcAccessMask,
			vk::AccessFlags2 a_dstAccessMask,
			vk::PipelineStageFlags2 a_srcStageMask,
			vk::PipelineStageFlags2 a_dstStageMask
		);

		Vector2u
		GetSize() const override;

		const StagingBuffer&
		GetStagingBuffer() const;

		const vk::raii::Image&
		GetVkImage() const;

		const vk::raii::DeviceMemory&
		GetVkDeviceMemory() const;

		const vk::raii::ImageView&
		GetVkImageView() const;

		ImageHandle
		GetHandle() const;

		operator ImageHandle() const
		{
			return GetHandle();
		}

	protected:
		Rendering::ImageHandle
		GetHandleImpl() const override
		{
			return GetHandle();
		}

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}
