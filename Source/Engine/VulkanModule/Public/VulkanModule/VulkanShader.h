#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "Rendering/Shader.h"

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

	using VulkanShaderId = IConvertibleDeviceObjectId<ShaderId, vk::Pipeline>;

	class VULKANMODULE_EXPORT VulkanShader : public IShader
	{
	public:
		VulkanShader(nullptr_t) {}

		explicit
		VulkanShader(const VulkanDevice& a_device, const ShaderCreateParams& a_params);

		DEFAULT_MOVE(VulkanShader);

		virtual
		~VulkanShader();

		void
		Destroy() override;

		bool
		IsValid() const override { return *m_pipeline; }

		const vk::raii::Pipeline&
		GetVkPipeline() const { return m_pipeline; }

		VulkanShaderId
		GetId() const { return *m_pipeline; }

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
		void
		CreatePipeline(const VulkanDevice& a_device, const ShaderCreateParams& a_params);

		vk::raii::ShaderModule
		CompileVkShaderModule(const VulkanDevice& a_device, const ShaderStage& a_stage);

	private:
		vk::raii::Pipeline m_pipeline = nullptr;
	};
}
