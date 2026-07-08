#pragma once

#include "Shader.h"

namespace Oyl::Rendering::Internal
{
	class OYL_RENDERING_API VulkanShader : public Shader
	{
	public:
		VulkanShader();

		virtual
		~VulkanShader();

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}