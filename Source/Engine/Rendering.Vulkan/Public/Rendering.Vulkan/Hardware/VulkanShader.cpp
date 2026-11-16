#include "VulkanShader.h"

#include <Core/Containers/Array.h>

#include "VulkanDevice.h"
#include "VulkanImage.h"
#include "VulkanUniformBuffer.h"

#include "Rendering/RenderTarget.h"

#include "Rendering.Vulkan/VulkanRenderingEnums.h"

#include <Core/Math/Matrix.h>

namespace Oyl
{
	static
	vk::Format
	GetFormatFromTypeId(TypeId a_typeId);

	VulkanShader::VulkanShader(const VulkanDevice& a_device, const RdoShaderCreateParams& a_params)
	{
		OYL_PROFILE_FUNCTION();

		CreatePipeline(a_device, a_params);
	}

	VulkanShader::~VulkanShader()
	{
		VulkanShader::Destroy();
	}

	void
	VulkanShader::Destroy()
	{
		if (!IsValid())
			return;

		m_vkPipeline.clear();
	}

	void
	VulkanShader::SetUniformBuffer(const IRenderDevice& a_device, const IRdoUniformBuffer& a_uniformBuffer)
	{
		auto& vulkanUniformBuffer = static_cast<const VulkanUniformBuffer&>(a_uniformBuffer);

		vk::DescriptorBufferInfo bufferInfo {
			.buffer = vulkanUniformBuffer.GetVkBuffer(),
			.offset = 0,
			.range = vk::WholeSize
		};
		vk::WriteDescriptorSet descriptorWrite {
			.dstSet = m_vkDescriptorSet,
			.dstBinding = 0,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType = vk::DescriptorType::eUniformBuffer,
			.pBufferInfo = &bufferInfo,
		};
		static_cast<const VulkanDevice&>(a_device).GetVkDevice().updateDescriptorSets(descriptorWrite, {});
	}

	void
	VulkanShader::CreatePipeline(const VulkanDevice& a_device, const RdoShaderCreateParams& a_params)
	{
		OYL_PROFILE_FUNCTION();

		auto& device = dynamic_cast<const VulkanDevice&>(a_device);
		auto& compileResult = a_params.compileResult;

		// keep ShaderModules for RAII
		std::vector<vk::raii::ShaderModule> vkShaderModules;
		std::vector<vk::PipelineShaderStageCreateInfo> vkShaderStageCreateInfos;
		for (const auto& stage : compileResult.GetShaderStages())
		{
			vk::raii::ShaderModule shaderModule = CompileVkShaderModule(device, stage);

			vk::PipelineShaderStageCreateInfo createInfo {
				.stage = ToVkEnum(stage.GetShaderProfile()),
				.module = shaderModule,
				.pName = stage.GetEntryPoint().data()
			};

			vkShaderModules.emplace_back(std::move(shaderModule));
			vkShaderStageCreateInfos.emplace_back(std::move(createInfo));
		}

		auto bindingDescription = vk::VertexInputBindingDescription {
			.binding = 0,
			.inputRate = vk::VertexInputRate::eVertex
		};
		Array<vk::VertexInputAttributeDescription> attributeDescriptions;
		for (const auto& stage : a_params.compileResult.GetShaderStages())
		{
			if (stage.GetShaderProfile() != ShaderProfileFlagBits::Vertex)
				continue;

			for (const auto& inputBinding : stage.GetInputBindings())
			{
				bindingDescription.stride += inputBinding.size;
				attributeDescriptions.Add({
					.location = inputBinding.location,
					.binding = 0,
					.format = GetFormatFromTypeId(inputBinding.type),
					.offset = inputBinding.offset
				});
			}
		}

		vk::PipelineVertexInputStateCreateInfo vertexInputInfo {
			.vertexBindingDescriptionCount = 1,
			.pVertexBindingDescriptions = &bindingDescription,
			.vertexAttributeDescriptionCount = static_cast<uint32>(attributeDescriptions.size()),
			.pVertexAttributeDescriptions = attributeDescriptions.data()
		};

		vk::PipelineInputAssemblyStateCreateInfo inputAssembly {
			.topology = vk::PrimitiveTopology::eTriangleList
		};

		std::vector dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };

		vk::PipelineDynamicStateCreateInfo dynamicState {
			.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
			.pDynamicStates = dynamicStates.data()
		};

		vk::PipelineViewportStateCreateInfo viewportState {
			.viewportCount = 1,
			.scissorCount = 1
		};

		vk::PipelineRasterizationStateCreateInfo rasterizer {
			.depthClampEnable = vk::False,
			.rasterizerDiscardEnable = vk::False,
			.polygonMode = vk::PolygonMode::eFill,
			.cullMode = vk::CullModeFlagBits::eBack,
			.frontFace = vk::FrontFace::eCounterClockwise,
			.depthBiasEnable = vk::False,
			.lineWidth = 1.0f,
		};

		vk::PipelineMultisampleStateCreateInfo multisampling {
			.rasterizationSamples = vk::SampleCountFlagBits::e1,
			.sampleShadingEnable = vk::False,
		};

		vk::PipelineColorBlendAttachmentState colorBlendAttachment {
			.blendEnable = vk::False,
			.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
			.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
			.colorBlendOp = vk::BlendOp::eAdd,
			.srcAlphaBlendFactor = vk::BlendFactor::eOne,
			.dstAlphaBlendFactor = vk::BlendFactor::eZero,
			.alphaBlendOp = vk::BlendOp::eAdd,
			.colorWriteMask = vk::ColorComponentFlagBits::eR
			                  | vk::ColorComponentFlagBits::eG
			                  | vk::ColorComponentFlagBits::eB
			                  | vk::ColorComponentFlagBits::eA,
		};

		vk::PipelineColorBlendStateCreateInfo colorBlending {
			.logicOpEnable = vk::False,
			.logicOp = vk::LogicOp::eCopy,
			.attachmentCount = 1,
			.pAttachments = &colorBlendAttachment
		};

		CreateDescriptorPool(device, a_params);
		CreateDescriptorSet(device, a_params);

		m_vkPipelineLayout = vk::raii::PipelineLayout(
			device.GetVkDevice(),
			{
				.setLayoutCount = 1,
				.pSetLayouts = &*m_vkDescriptorSetLayout,
				.pushConstantRangeCount = 0,
			});

		{
			OYL_PROFILE_SCOPE("vk::raii::Pipeline");

			Array<vk::Format> colorAttachmentFormats;
			for (uint32 i = 0; i < a_params.renderTarget.GetNumColorAttachments(); i++)
			{
				auto* colorAttachment = a_params.renderTarget.GetColorAttachment(i);
				auto& vulkanColorAttachment = dynamic_cast<const VulkanImage&>(*colorAttachment);
				colorAttachmentFormats.Add(vulkanColorAttachment.GetVkFormat());
			}
			vk::Format depthAttachmentFormat = vk::Format::eUndefined;
			if (auto* depthAttachment = a_params.renderTarget.GetDepthAttachment())
			{
				auto& vulkanDepthAttachment = dynamic_cast<const VulkanImage&>(*depthAttachment);
				depthAttachmentFormat = vulkanDepthAttachment.GetVkFormat();
			}

			// Use structure chain to auto-populate pNext
			vk::StructureChain pipelineCreateInfoChain {
				vk::GraphicsPipelineCreateInfo {
					.stageCount = (uint32) vkShaderStageCreateInfos.size(),
					.pStages = vkShaderStageCreateInfos.data(),
					.pVertexInputState = &vertexInputInfo,
					.pInputAssemblyState = &inputAssembly,
					.pViewportState = &viewportState,
					.pRasterizationState = &rasterizer,
					.pMultisampleState = &multisampling,
					.pColorBlendState = &colorBlending,
					.pDynamicState = &dynamicState,
					.layout = m_vkPipelineLayout,
					.renderPass = nullptr
				},
				vk::PipelineRenderingCreateInfo {
					.colorAttachmentCount = colorAttachmentFormats.Size(),
					.pColorAttachmentFormats = colorAttachmentFormats.Data(),
					.depthAttachmentFormat = depthAttachmentFormat
				}
			};

			m_vkPipeline = vk::raii::Pipeline(
				device.GetVkDevice(),
				nullptr,
				pipelineCreateInfoChain.get()
			);
		}
	}

	vk::raii::ShaderModule
	VulkanShader::CompileVkShaderModule(const VulkanDevice& a_device, const ShaderStage& a_stage)
	{
		OYL_PROFILE_FUNCTION();

		const auto& bytecode = a_stage.GetByteCode();

		// Create a Vulkan shader module from the compilation result
		vk::ShaderModuleCreateInfo shaderModuleCreateInfo {
			.codeSize = bytecode.size(),
			.pCode = (uint32*) bytecode.data(),
		};
		return vk::raii::ShaderModule(a_device.GetVkDevice(), shaderModuleCreateInfo);
	}

	void
	VulkanShader::CreateDescriptorPool(const VulkanDevice& a_device, const RdoShaderCreateParams& a_params)
	{
		vk::DescriptorPoolSize poolSize {
			.type = vk::DescriptorType::eUniformBuffer,
			.descriptorCount = [&]
			{
				uint result = 0;
				for (const auto& stage : a_params.compileResult.GetShaderStages())
					result += stage.GetBufferBindings().Length();
				return result;
			}(),
		};

		vk::DescriptorPoolCreateInfo poolInfo = {
			.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
			.maxSets = 1,
			.poolSizeCount = 1,
			.pPoolSizes = &poolSize,
		};

		m_vkDescriptorPool = vk::raii::DescriptorPool(a_device.GetVkDevice(), poolInfo);
	}

	void
	VulkanShader::CreateDescriptorSet(const VulkanDevice& a_device, const RdoShaderCreateParams& a_params)
	{
		Array<vk::DescriptorSetLayoutBinding> uboBindings;
		for (const auto& stage : a_params.compileResult.GetShaderStages())
		{
			if (stage.GetBufferBindings().Empty())
				continue;

			for (const auto& bufferLayoutBinding : stage.GetBufferBindings())
			{
				vk::DescriptorSetLayoutBinding uboLayoutBinding {
					.binding = bufferLayoutBinding.binding,
					.descriptorType = vk::DescriptorType::eUniformBuffer,
					.descriptorCount = stage.GetBufferBindings().Length(),
				};
				if (stage.GetShaderProfile() == ShaderProfileFlagBits::Vertex)
					uboLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;
				if (stage.GetShaderProfile() == ShaderProfileFlagBits::Fragment)
					uboLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;
				uboBindings.Add(uboLayoutBinding);
			}
		}
		m_vkDescriptorSetLayout = vk::raii::DescriptorSetLayout(
			a_device.GetVkDevice(),
			{
				.bindingCount = uboBindings.Length(),
				.pBindings = uboBindings.Data(),
			});

		vk::DescriptorSetAllocateInfo allocInfo {
			.descriptorPool = m_vkDescriptorPool,
			.descriptorSetCount = 1,
			.pSetLayouts = &*m_vkDescriptorSetLayout
		};
		m_vkDescriptorSet = std::move(a_device.GetVkDevice().allocateDescriptorSets(allocInfo).front());
	}

	vk::Format
	GetFormatFromTypeId(TypeId a_typeId)
	{
		static ArrayMap<TypeId, vk::Format> formats = []
		{
			ArrayMap<TypeId, vk::Format> result;
			result[typeidof(float)] = vk::Format::eR32Sfloat;
			result[typeidof(Vector_t<2, float>)] = vk::Format::eR32G32Sfloat;
			result[typeidof(Vector_t<3, float>)] = vk::Format::eR32G32B32Sfloat;
			result[typeidof(Vector_t<4, float>)] = vk::Format::eR32G32B32A32Sfloat;
			return result;
		}();

		OYL_ASSERT(formats.ContainsKey(a_typeId));
		return formats[a_typeId];
	}

}
