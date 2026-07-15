#pragma once

#include <vulkan/vulkan_raii.hpp>

#include <memory>

#include "VertexBuffer.h"

namespace Oyl::Rendering::Vulkan
{
	struct VertexBufferParams
	{
		vk::raii::Device& device;
		vk::raii::PhysicalDevice& physicalDevice;
		vk::raii::CommandPool& commandPool;
		vk::raii::Queue& queue;
	};

	class OYL_RENDERING_API VertexBufferResource : public Rendering::VertexBufferResource
	{
	public:
		VertexBufferResource();

		VertexBufferResource(const byte* a_data, size_t a_size);

		virtual
		~VertexBufferResource();

		bool
		Load() override;

		bool
		Unload() override;

		bool
		DeviceLoad(void* a_params) override;

		bool
		DeviceUnload(void* a_params) override;

		const vk::raii::Buffer&
		GetVertexBuffer();

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};

	using VertexBuffer = ResourceHandle<VertexBufferResource>;
}
