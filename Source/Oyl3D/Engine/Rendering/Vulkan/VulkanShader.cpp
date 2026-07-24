#include "VulkanShader.h"

#include <vulkan/vulkan_raii.hpp>

#include "VulkanDevice.h"
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

		vk::Format format;

		vk::raii::Pipeline pipeline = nullptr;

		vk::raii::ShaderModule
		CompileVkShaderModule(
			const Device& a_device,
			const ShaderStage& a_shaderStage
		);
	};

	ShaderResource::ShaderResource()
	{
		Init();
	}

	ShaderResource::ShaderResource(ShaderOptions a_options, vk::Format a_format)
		: Rendering::ShaderResource(std::move(a_options))
	{
		Init();
		SetVkFormat(a_format);
	}

	void
	ShaderResource::Init()
	{
		m_impl = std::make_unique<Impl>();
	}

	ShaderResource::~ShaderResource() {}

	vk::Format
	ShaderResource::GetVkFormat() const
	{
		return m_impl->format;
	}

	void
	ShaderResource::SetVkFormat(vk::Format a_format)
	{
		m_impl->format = a_format;
		SetDeviceDirty();
	}

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
	ShaderResource::DeviceLoad(const Rendering::Device& a_device)
	{
		OYL_PROFILE_FUNCTION();

		const auto& device = static_cast<const Device&>(a_device);

		// keep ShaderModules for RAII
		std::vector<vk::raii::ShaderModule> vkShaderModules;
		std::vector<vk::PipelineShaderStageCreateInfo> vkShaderStageCreateInfos;
		for (const auto& stage : m_impl->compileResult.GetShaderStages())
		{
			vk::raii::ShaderModule shaderModule = m_impl->CompileVkShaderModule(device, stage);

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
					.pColorAttachmentFormats = &m_impl->format
				}
			};

			OYL_PROFILE_SCOPE("vk::raii::Pipeline");
			m_impl->pipeline = vk::raii::Pipeline(
				device.GetVkDevice(),
				nullptr,
				pipelineCreateInfoChain.get()
			);
		}

		return Rendering::ShaderResource::DeviceLoad(a_device);
	}

	bool
	ShaderResource::DeviceUnload()
	{
		OYL_PROFILE_FUNCTION();

		m_impl->pipeline.clear();

		return Rendering::ShaderResource::DeviceUnload();
	}

	const vk::raii::Pipeline&
	ShaderResource::GetPipeline() const
	{
		return m_impl->pipeline;
	}

	vk::raii::ShaderModule
	ShaderResource::Impl::CompileVkShaderModule(
		const Device& a_device,
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
		return vk::raii::ShaderModule(a_device.GetVkDevice(), shaderModuleCreateInfo);
	}
}
