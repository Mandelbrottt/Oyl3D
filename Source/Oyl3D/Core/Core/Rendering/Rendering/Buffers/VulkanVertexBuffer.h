#pragma once

#include <vulkan/vulkan_raii.hpp>

#include <memory>

#include "VertexBuffer.h"

namespace Oyl::Rendering::Vulkan
{
	struct VertexBufferParams
	{
		const vk::raii::Device& device;
		const vk::raii::PhysicalDevice& physicalDevice;
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
