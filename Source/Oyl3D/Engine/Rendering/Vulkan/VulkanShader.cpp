#include "VulkanShader.h"

#include <vulkan/vulkan_raii.hpp>

#include "VulkanShaderCompiler.h"

#include "Rendering/RenderEngine.h"

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

	struct ShaderResource::Impl
	{
		ShaderCompileResult compileResult;

		vk::raii::Pipeline pipeline = nullptr;

		vk::raii::ShaderModule
		CompileShaderModule(
			const vk::raii::Device& a_device,
			const ShaderStage& a_shaderStage
		);
	};

	ShaderResource::ShaderResource()
	{
		Init();
	}

	ShaderResource::ShaderResource(ShaderOptions a_options)
		: Rendering::ShaderResource(std::move(a_options))
	{
		Init();
	}

	void
	ShaderResource::Init()
	{
		m_impl = std::make_unique<Impl>();
	}

	ShaderResource::~ShaderResource() {}

	bool
	ShaderResource::Load()
	{
		OYL_PROFILE_FUNCTION();

		if (IsLoaded())
			Unload();

		if (GetFilePath().empty())
			return false;

		auto* shaderCompiler = RenderEngine::GetShaderCompiler();
		OYL_ASSERT(shaderCompiler);
		shaderCompiler->CompileHlslFromFile(m_options.filepath, &m_impl->compileResult);

		return Rendering::ShaderResource::Load();
	}

	bool
	ShaderResource::Unload()
	{
		OYL_PROFILE_FUNCTION();

		if (!IsLoaded())
			return true;

		m_impl->compileResult = {};

		return Rendering::ShaderResource::Unload();
	}

	static
	vk::ShaderStageFlagBits
	ShaderProfileToVkShaderStageFlag(ShaderProfile a_profile)
	{
		vk::ShaderStageFlagBits flags[SP_Count];
		flags[SP_Vertex] = vk::ShaderStageFlagBits::eVertex;
		flags[SP_Geometry] = vk::ShaderStageFlagBits::eGeometry;
		flags[SP_Fragment] = vk::ShaderStageFlagBits::eFragment;
		return flags[a_profile];
	}

	bool
	ShaderResource::DeviceLoad(void* a_params)
	{
		OYL_PROFILE_FUNCTION();

		const ShaderDeviceLoadParams& params = *static_cast<ShaderDeviceLoadParams*>(a_params);

		// keep ShaderModules for RAII
		std::vector<vk::raii::ShaderModule> vkShaderModules;
		std::vector<vk::PipelineShaderStageCreateInfo> vkShaderStageCreateInfos;
		for (const auto& stage : m_impl->compileResult.GetShaderStages())
		{
			vk::raii::ShaderModule shaderModule = m_impl->CompileShaderModule(params.device, stage);

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

		auto pipelineLayout = vk::raii::PipelineLayout(params.device, pipelineLayoutInfo);

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
				.pColorAttachmentFormats = &params.format
			}
		};

		{
			OYL_PROFILE_SCOPE("vk::raii::Pipeline");
			m_impl->pipeline = vk::raii::Pipeline(
				params.device,
				nullptr,
				pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>()
			);
		}

		return Rendering::ShaderResource::DeviceLoad(a_params);
	}

	bool
	ShaderResource::DeviceUnload(void* a_params)
	{
		OYL_PROFILE_FUNCTION();

		return Rendering::ShaderResource::DeviceUnload(a_params);
	}

	const vk::raii::Pipeline&
	ShaderResource::GetPipeline() const
	{
		return m_impl->pipeline;
	}

	vk::raii::ShaderModule
	ShaderResource::Impl::CompileShaderModule(
		const vk::raii::Device& a_device,
		const ShaderStage& a_shaderStage
	)
	{
		OYL_PROFILE_FUNCTION();

		const auto& bytecode = a_shaderStage.GetByteCode();

		// Create a Vulkan shader module from the compilation result
		vk::ShaderModuleCreateInfo shaderModuleCreateInfo {
			.codeSize = bytecode.size(),
			.pCode = (uint32*) bytecode.data(),
		};
		return vk::raii::ShaderModule(a_device, shaderModuleCreateInfo);
	}
}
