#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "Shader.h"

namespace Oyl::Rendering::Vulkan
{
	struct ShaderDeviceLoadParams : DeviceLoadParams
	{
		const vk::raii::Device& device;
		vk::Format format = vk::Format::eUndefined;
	};

	struct ShaderDeviceUnloadParams : DeviceUnloadParams
	{
		const vk::raii::Device& device;
	};

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

		ShaderResource(std::string_view a_filePath, ShaderCompiler* a_shaderCompiler);

		virtual
		~ShaderResource();

		const vk::raii::Pipeline&
		GetPipeline() const;

		bool
		Load() override;

		bool
		Unload() override;

		bool
		DeviceLoad(void* a_params) override;

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
