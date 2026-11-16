#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "Rendering/Hardware/RdoShader.h"

namespace Oyl
{
	class VulkanDevice;

	using VulkanShaderId = IConvertibleRenderDeviceObjectId<RdoShaderId, vk::Pipeline>;

	class RENDERING_VULKAN_EXPORT VulkanShader : public IRdoShader
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

		const vk::raii::Pipeline&
		GetVkPipeline() const { return m_vkPipeline; }

		const vk::raii::PipelineLayout&
		GetVkPipelineLayout() const { return m_vkPipelineLayout; }

		const vk::raii::DescriptorSet&
		GetVkDescriptorSet() const { return m_vkDescriptorSet; }

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
		CreateDescriptorPool(const VulkanDevice& a_device, const RdoShaderCreateParams& a_params);

		void
		CreateDescriptorSet(const VulkanDevice& a_device, const RdoShaderCreateParams& a_params);

	private:
		vk::raii::PipelineLayout m_vkPipelineLayout = nullptr;
		vk::raii::Pipeline m_vkPipeline = nullptr;

		vk::raii::DescriptorPool m_vkDescriptorPool = nullptr;
		vk::raii::DescriptorSet m_vkDescriptorSet = nullptr;
		vk::raii::DescriptorSetLayout m_vkDescriptorSetLayout = nullptr;
	};
}
