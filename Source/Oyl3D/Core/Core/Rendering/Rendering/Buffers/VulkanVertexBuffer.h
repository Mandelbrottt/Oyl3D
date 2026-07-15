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

		VertexBufferResource(const byte* a_vertexData, size_t a_vertexLength);

		VertexBufferResource(
			const byte* a_vertexData,
			size_t a_vertexLength,
			const byte* a_indexData,
			size_t a_indexLength
		);

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
		GetVkBuffer();

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};

	using VertexBuffer = ResourceHandle<VertexBufferResource>;
}
