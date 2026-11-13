#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "Rendering/Hardware/Shader.h"

namespace Oyl
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

	using VulkanShaderId = IConvertibleRenderDeviceObjectId<RdoShaderId, vk::Pipeline>;

	class VULKANMODULE_EXPORT VulkanShader : public IRdoShader
	{
	public:
		VulkanShader(nullptr_t) {}

		explicit
		VulkanShader(const VulkanDevice& a_device, const RdoShaderCreateParams& a_params);

		DEFAULT_MOVE(VulkanShader);

		virtual
		~VulkanShader();

		void
		Destroy() override;

		bool
		IsValid() const override { return *m_vkPipeline; }

		void
		SetUniformBuffer(const IRenderDevice& a_device, const IRdoUniformBuffer& a_uniformBuffer) override;

		void
		BindUniformBuffer(const IRdoCommandBuffer& a_commandBuffer, const IRdoUniformBuffer& a_uniformBuffer) override;

		const vk::raii::Pipeline&
		GetVkPipeline() const { return m_vkPipeline; }

		VulkanShaderId
		GetId() const { return *m_vkPipeline; }

		operator VulkanShaderId() const
		{
			return GetId();
		}

	protected:
		RdoShaderId
		GetIdImpl() const override
		{
			return GetId();
		}

	private:
		void
		CreatePipeline(const VulkanDevice& a_device, const RdoShaderCreateParams& a_params);

		vk::raii::ShaderModule
		CompileVkShaderModule(const VulkanDevice& a_device, const ShaderStage& a_stage);

		void
		CreateDescriptorPool(const VulkanDevice& a_device);

		void
		CreateDescriptorSet(const VulkanDevice& a_device);

	private:
		vk::raii::PipelineLayout m_vkPipelineLayout = nullptr;
		vk::raii::Pipeline m_vkPipeline = nullptr;

		vk::raii::DescriptorPool m_vkDescriptorPool = nullptr;
		vk::raii::DescriptorSet m_vkDescriptorSet = nullptr;
		vk::raii::DescriptorSetLayout m_vkDescriptorSetLayout = nullptr;
	};
}
