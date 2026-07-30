#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "Rendering/Shader.h"
#include "Rendering/ShaderCompiler.h"

namespace Oyl::Rendering::Vulkan
{
	class DeviceImpl;

	struct Vertex : Rendering::Vertex
	{
		static
		vk::VertexInputBindingDescription
		GetBindingDescription();

		static
		std::array<vk::VertexInputAttributeDescription, 2>
		GetAttributeDescriptions();
	};

	using ShaderHandle = OpaqueHandleConvertible<ShaderHandle, vk::Pipeline>;

	class OYL_RENDERING_API Shader : public Rendering::Shader
	{
	public:
		Shader();

		struct CreateParams
		{
			const DeviceImpl& device;
			vk::Format format;

			ShaderCompileResult compileResult;
		};

		explicit
		Shader(const CreateParams& a_params);

		Shader(Shader&& a_other) noexcept;
		Shader&
		operator =(Shader&& a_other) noexcept;

		virtual
		~Shader();

		void
		Destroy() override;

		bool
		IsValid() const override;

		const vk::raii::Pipeline&
		GetVkPipeline() const;

		ShaderHandle
		GetHandle() const;

		operator ShaderHandle() const
		{
			return GetHandle();
		}

	protected:
		Rendering::ShaderHandle
		GetHandleImpl() const override
		{
			return GetHandle();
		}

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}
