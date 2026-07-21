#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "Rendering/Shader.h"

namespace Oyl::Rendering::Vulkan
{
	class Device;

	struct Vertex : Rendering::Vertex
	{
		static
		vk::VertexInputBindingDescription
		GetBindingDescription();

		static
		std::array<vk::VertexInputAttributeDescription, 2>
		GetAttributeDescriptions();
	};

	class ShaderCompiler;

	class OYL_RENDERING_API ShaderResource : public Rendering::ShaderResource
	{
	public:
		ShaderResource();

		explicit
		ShaderResource(ShaderOptions a_options);

		virtual
		~ShaderResource();

		const vk::raii::Pipeline&
		GetPipeline() const;

		bool
		Load() override;

		bool
		Unload() override;

		struct DeviceLoadParams : Oyl::DeviceLoadParams
		{
			const Device& device;
			vk::Format format = vk::Format::eUndefined;
		};

		bool
		DeviceLoad(void* a_params) override;

		struct DeviceUnloadParams : Oyl::DeviceUnloadParams
		{
			const Device& device;
		};

		bool
		DeviceUnload(void* a_params) override;

	private:
		void
		Init();

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};

	using Shader = ResourceHandle<ShaderResource>;
}
