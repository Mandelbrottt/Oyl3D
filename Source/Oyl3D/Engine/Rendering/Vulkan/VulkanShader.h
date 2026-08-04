#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "Rendering/Shader.h"

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

	class OYL_RENDERING_API ShaderImpl : public Rendering::ShaderImpl
	{
	public:
		ShaderImpl();

		explicit
		ShaderImpl(const DeviceImpl& a_device, const CreateParams& a_params);

		ShaderImpl(ShaderImpl&& a_other) noexcept;
		ShaderImpl&
		operator =(ShaderImpl&& a_other) noexcept;

		virtual
		~ShaderImpl();

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

	using Shader = PImpl<ShaderImpl>;
}
