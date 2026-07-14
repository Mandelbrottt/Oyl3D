#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "Shader.h"

namespace Oyl::Rendering::Vulkan
{
	struct ShaderCompileInput
	{
		const vk::raii::Device& device;
		vk::Format format = vk::Format::eUndefined;
	};

	class OYL_RENDERING_API ShaderResource : public Rendering::ShaderResource
	{
	public:
		ShaderResource();

		virtual
		~ShaderResource();

		bool
		Load() override;

		bool
		Unload() override;

		const vk::raii::Pipeline&
		GetPipeline() const;

		bool
		Compile(const ShaderCompileInput& a_input);

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};

	using Shader = ResourceHandle<ShaderResource>;
}
