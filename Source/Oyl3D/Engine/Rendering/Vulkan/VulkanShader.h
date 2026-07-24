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
		ShaderResource(ShaderOptions a_options, vk::Format a_format);

		virtual
		~ShaderResource();

		vk::Format
		GetVkFormat() const;

		void
		SetVkFormat(vk::Format a_format);

		const vk::raii::Pipeline&
		GetPipeline() const;

		bool
		Load() override;

		bool
		Unload() override;

		bool
		DeviceLoad(const Rendering::Device& a_device) override;

		bool
		DeviceUnload() override;

	private:
		void
		Init();

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};

	using Shader = ResourceHandle<ShaderResource>;
}
