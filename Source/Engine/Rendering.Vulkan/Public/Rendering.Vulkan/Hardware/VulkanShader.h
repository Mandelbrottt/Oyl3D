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

		const vk::raii::Pipeline&
		GetVkPipeline() const { return m_vkPipeline; }

		const vk::raii::PipelineLayout&
		GetVkPipelineLayout() const { return m_vkPipelineLayout; }

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
		CreateDescriptorSetLayout(const VulkanDevice& a_device, const RdoShaderCreateParams& a_params);

	private:
		vk::raii::PipelineLayout m_vkPipelineLayout = nullptr;
		vk::raii::Pipeline m_vkPipeline = nullptr;

		vk::raii::DescriptorSetLayout m_vkDescriptorSetLayout = nullptr;

		Array<ShaderBufferBinding> m_bufferBindings;
	};
}
