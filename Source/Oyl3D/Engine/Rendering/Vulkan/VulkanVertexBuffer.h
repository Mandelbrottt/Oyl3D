#pragma once

#include <vulkan/vulkan_raii.hpp>

#include <memory>

#include "Rendering/VertexBuffer.h"

namespace Oyl::Rendering::Vulkan
{
	class OYL_RENDERING_API VertexBufferResource : public Rendering::VertexBufferResource
	{
	public:
		VertexBufferResource();

		explicit
		VertexBufferResource(const VertexBufferOptions& a_options);

		virtual
		~VertexBufferResource();

		bool
		Load() override;

		bool
		Unload() override;

		struct DeviceLoadParams
		{
			const vk::raii::Device& device;
			const vk::raii::PhysicalDevice& physicalDevice;
			const vk::raii::CommandPool& commandPool;
			const vk::raii::Queue& queue;
		};

		bool
		DeviceLoad(void* a_params) override;

		bool
		DeviceUnload(void* a_params) override;

		const vk::raii::Buffer&
		GetVkBuffer();

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};

	using VertexBuffer = ResourceHandle<VertexBufferResource>;
}
