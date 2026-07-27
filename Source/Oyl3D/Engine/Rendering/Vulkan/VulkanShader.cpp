#include "VulkanShader.h"

#include "VulkanDevice.h"

namespace Oyl::Rendering::Vulkan
{
	vk::VertexInputBindingDescription
	Vertex::GetBindingDescription()
	{
		return vk::VertexInputBindingDescription {
			.binding = 0,
			.stride = sizeof(Vertex),
			.inputRate = vk::VertexInputRate::eVertex
		};
	}

	std::array<vk::VertexInputAttributeDescription, 2>
	Vertex::GetAttributeDescriptions()
	{
		return {
			vk::VertexInputAttributeDescription {
				.location = 0,
				.binding = 0,
				.format = vk::Format::eR32G32Sfloat,
				.offset = offsetof(Vertex, position)
			},
			vk::VertexInputAttributeDescription {
				.location = 1,
				.binding = 0,
				.format = vk::Format::eR32G32B32Sfloat,
				.offset = offsetof(Vertex, color)
			},
		};
	}

	struct Shader::Impl
	{
		vk::raii::Pipeline pipeline = nullptr;

		void
		CreatePipeline(const CreateParams& a_params);

		vk::raii::ShaderModule
		CompileVkShaderModule(const Device& a_device, const ShaderStage& a_stage);
	};

	Shader::Shader()
		: m_impl(nullptr) {}

	Shader::Shader(const CreateParams& a_params)
		: m_impl(std::make_unique<Impl>())
	{
		OYL_PROFILE_FUNCTION();

		m_impl->CreatePipeline(a_params);
	}

	Shader::Shader(Shader&& a_other) noexcept
	{
		*this = std::move(a_other);
	}

	Shader&
	Shader::operator=(Shader&& a_other) noexcept
	{
		if (this != &a_other)
		{
			std::swap(m_impl, a_other.m_impl);
		}
		return *this;
	}

	Shader::~Shader()
	{
		Shader::Destroy();
	}

	void
	Shader::Destroy()
	{
		if (!IsValid())
			return;

		m_impl->pipeline.clear();
	}

	bool
	Shader::IsValid() const
	{
		return m_impl
		       && *m_impl->pipeline;
	}

	Rendering::ShaderHandle
	Shader::GetHandleImpl() const
	{
		return (ShaderHandle) *m_impl->pipeline;
	}

	const vk::raii::Pipeline&
	Shader::GetVkPipeline() const
	{
		return m_impl->pipeline;
	}

	static
	vk::ShaderStageFlagBits
	ShaderProfileToVkShaderStageFlag(ShaderProfile a_profile);

	void
	Shader::Impl::CreatePipeline(const CreateParams& a_params)
	{
		OYL_PROFILE_FUNCTION();

		auto& device = a_params.device;
		auto& format = a_params.format;
		auto& compileResult = a_params.compileResult;

		// keep ShaderModules for RAII
		std::vector<vk::raii::ShaderModule> vkShaderModules;
		std::vector<vk::PipelineShaderStageCreateInfo> vkShaderStageCreateInfos;
		for (const auto& stage : compileResult.GetShaderStages())
		{
			vk::raii::ShaderModule shaderModule = CompileVkShaderModule(device, stage);

			vk::PipelineShaderStageCreateInfo createInfo {
				.stage = ShaderProfileToVkShaderStageFlag(stage.GetShaderProfile()),
				.module = shaderModule,
				.pName = stage.GetEntryPoint().data()
			};

			vkShaderModules.emplace_back(std::move(shaderModule));
			vkShaderStageCreateInfos.emplace_back(std::move(createInfo));
		}

		auto bindingDescription = Vertex::GetBindingDescription();
		auto attributeDescriptions = Vertex::GetAttributeDescriptions();

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
			.frontFace = vk::FrontFace::eClockwise,
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

		vk::PipelineLayoutCreateInfo pipelineLayoutInfo {
			.setLayoutCount = 0,
			.pushConstantRangeCount = 0
		};

		auto pipelineLayout = vk::raii::PipelineLayout(device.GetVkDevice(), pipelineLayoutInfo);

		{
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
					.layout = pipelineLayout,
					.renderPass = nullptr
				},
				vk::PipelineRenderingCreateInfo {
					.colorAttachmentCount = 1,
					.pColorAttachmentFormats = &format
				}
			};

			OYL_PROFILE_SCOPE("vk::raii::Pipeline");
			pipeline = vk::raii::Pipeline(
				device.GetVkDevice(),
				nullptr,
				pipelineCreateInfoChain.get()
			);
		}
	}

	vk::raii::ShaderModule
	Shader::Impl::CompileVkShaderModule(const Device& a_device, const ShaderStage& a_stage)
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

	vk::ShaderStageFlagBits
	ShaderProfileToVkShaderStageFlag(ShaderProfile a_profile)
	{
		vk::ShaderStageFlagBits flags[SP_Count];
		flags[SP_Vertex] = vk::ShaderStageFlagBits::eVertex;
		flags[SP_Geometry] = vk::ShaderStageFlagBits::eGeometry;
		flags[SP_Fragment] = vk::ShaderStageFlagBits::eFragment;
		return flags[a_profile];
	}
}
