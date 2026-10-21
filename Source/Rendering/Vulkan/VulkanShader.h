#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "Rendering/Shader.h"

namespace Oyl::Rendering
{
	class VulkanDevice;

	struct VulkanVertex : Vertex
	{
		static
		vk::VertexInputBindingDescription
		GetBindingDescription();

		static
		std::array<vk::VertexInputAttributeDescription, 2>
		GetAttributeDescriptions();
	};

	using VulkanShaderId = OpaqueHandleConvertible<ShaderId, vk::Pipeline>;

	class OYL_RENDERING_API VulkanShader : public Shader
	{
	public:
		VulkanShader();

		explicit
		VulkanShader(const VulkanDevice& a_device, const CreateParams& a_params);

		VulkanShader(VulkanShader&& a_other) noexcept;
		VulkanShader&
		operator =(VulkanShader&& a_other) noexcept;

		virtual
		~VulkanShader();

		void
		Destroy() override;

		bool
		IsValid() const override;

		const vk::raii::Pipeline&
		GetVkPipeline() const;

		VulkanShaderId
		GetId() const;

		operator VulkanShaderId() const
		{
			return GetId();
		}

	protected:
		ShaderId
		GetIdImpl() const override
		{
			return GetId();
		}

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};

	using VulkanShaderHandle = PImpl<VulkanShader>;
}
