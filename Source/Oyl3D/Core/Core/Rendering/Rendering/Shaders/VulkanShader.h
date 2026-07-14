#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "Shader.h"

namespace Oyl::Rendering::Internal
{
	struct VulkanShaderCompileInput
	{
		const vk::raii::Device& device;
		vk::Format format = vk::Format::eUndefined;
	};

	class OYL_RENDERING_API VulkanShaderResource : public ShaderResource
	{
	public:
		VulkanShaderResource();

		virtual
		~VulkanShaderResource();

		bool
		Load() override;

		bool
		Unload() override;

		const vk::raii::Pipeline&
		GetPipeline() const;

		bool
		Compile(const VulkanShaderCompileInput& a_input);

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};

	using VulkanShader = ResourceHandle<VulkanShaderResource>;
}
