#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "Rendering/Image.h"

namespace Oyl::Rendering::Vulkan
{
	class DeviceImpl;
	class StagingBuffer;
	class CommandBufferImpl;

	using ImageHandle = OpaqueHandleConvertible<ImageHandle, vk::Image>;

	class OYL_RENDERING_API ImageImpl : public Rendering::ImageImpl
	{
	public:
		ImageImpl(nullptr_t);

		ImageImpl(const DeviceImpl& a_device, const CreateParams& a_params);

		ImageImpl(ImageImpl&& a_other) noexcept;
		ImageImpl&
		operator =(ImageImpl&& a_other) noexcept;

		virtual
		~ImageImpl();

		void
		Destroy() override;

		bool
		IsValid() const override;

		void
		VkTransitionImageLayout(
			const CommandBufferImpl& a_commandBuffer,
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

		ImageFormat
		GetFormat() const override;

		const vk::raii::Image&
		GetVkImage() const;

		vk::Format
		GetVkFormat() const;

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

	using Image = PImpl<ImageImpl>;
}
