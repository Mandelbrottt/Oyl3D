#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "Rendering/Image.h"

namespace Oyl::Rendering::Vulkan
{
	class Device;
	class StagingBuffer;

	using ImageHandle = OpaqueHandleConvertible<ImageHandle, vk::Image>;

	class OYL_RENDERING_API Image : public Rendering::Image
	{
	public:
		Image();

		struct CreateParams
		{
			const Device& device;

			const byte* pixelData;
			uint32 pixelLength;

			Vector2u size;

			vk::Format vkFormat;

			vk::ImageUsageFlags vkUsage;
			vk::MemoryPropertyFlags vkProperties;
		};

		explicit
		Image(const CreateParams& a_params);

		Image(Image&& a_other) noexcept;
		Image&
		operator =(Image&& a_other) noexcept;

		virtual
		~Image();

		void
		Destroy() override;

		bool
		IsValid() const override;

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
