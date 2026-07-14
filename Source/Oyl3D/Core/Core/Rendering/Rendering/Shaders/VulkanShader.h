#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "Shader.h"

namespace Oyl::Rendering::Internal
{
	struct ShaderCompileInput
	{
		vk::raii::Device* device;
		vk::Format format;
		std::string_view filePath;
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
		Compile(void* a_shaderCompileInput) override;

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}