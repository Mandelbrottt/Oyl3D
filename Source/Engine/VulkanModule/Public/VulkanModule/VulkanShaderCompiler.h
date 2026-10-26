#pragma once

#include <memory>

#include <Core/Containers/UniquePtr.h>

#include "Rendering/ShaderCompiler.h"

namespace Oyl
{
	class VULKANMODULE_EXPORT VulkanShaderCompiler : public IShaderCompiler
	{
	public:
		VulkanShaderCompiler();

		VulkanShaderCompiler(const VulkanShaderCompiler& a_other);
		VulkanShaderCompiler&
		operator =(const VulkanShaderCompiler& a_other);

		VulkanShaderCompiler(VulkanShaderCompiler&& a_other) noexcept;
		VulkanShaderCompiler&
		operator =(VulkanShaderCompiler&& a_other) noexcept;

		virtual
		~VulkanShaderCompiler();

	private:
		bool
		CompileHlsl(StringView a_source, ShaderCompileResult* a_outShader) const override;

	private:
		struct Impl;
		UniquePtr<Impl> m_impl;
	};
}
