#include "VulkanShader.h"

#if defined(OYL_WINDOWS)
#include <atlcomcli.h>
#include <Windows.h>
#endif

#include <vulkan/vulkan_raii.hpp>

#include <Core/Application/SharedLibrary.h>

#include <dxc/dxcapi.h>

namespace Oyl::Rendering::Vulkan
{
	std::wstring_view
	EntryPointFromProfile(ShaderProfile a_profile)
	{
		switch (a_profile)
		{
			case SP_Vertex:
				return L"VertMain";
			case SP_Fragment:
				return L"FragMain";
			case SP_Count: break;
		}

		return L"";
	}

	std::wstring_view
	ProfileStringFromProfile(ShaderProfile a_profile)
	{
		switch (a_profile)
		{
			case SP_Vertex:
				return L"vs_6_4";
			case SP_Fragment:
				return L"ps_6_4"; // HLSL uses Pixel Shader instead of Fragment Shader
			case SP_Count: break;
		}

		return L"";
	}

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
		std::vector<std::vector<byte>> shaderByteCodeBuffers;

		vk::raii::Pipeline pipeline = nullptr;

		// For compiling hlsl into spir-v
		SharedLibrary dxcCompilerLibrary;
		decltype(DxcCreateInstance)* dxcCreateInstanceFn;

		std::vector<byte>
		CompileShaderBytecode(ShaderProfile a_profile, std::string_view a_filePath);

		vk::raii::ShaderModule
		CompileShaderModule(
			const vk::raii::Device& a_device,
			ShaderProfile a_profile
		);
	};

	ShaderResource::ShaderResource()
	{
		Init();
	}

	ShaderResource::ShaderResource(std::string_view a_filePath)
		: Rendering::ShaderResource(a_filePath)
	{
		Init();
	}

	void
	ShaderResource::Init()
	{
		m_impl = std::make_unique<Impl>();

		m_impl->dxcCompilerLibrary = SharedLibrary("dxcompiler.dll");
		OYL_ASSERT(m_impl->dxcCompilerLibrary.IsLoaded());

		m_impl->dxcCreateInstanceFn =
			m_impl->dxcCompilerLibrary.GetFunction<decltype(DxcCreateInstance)>("DxcCreateInstance");
		OYL_ASSERT(m_impl->dxcCreateInstanceFn);
	}

	ShaderResource::~ShaderResource() {}

	bool
	ShaderResource::Load()
	{
		if (IsLoaded())
			Unload();

		if (GetFilePath().empty())
			return false;

		m_impl->shaderByteCodeBuffers.resize(SP_Count);
		m_impl->shaderByteCodeBuffers[SP_Vertex] = m_impl->CompileShaderBytecode(SP_Vertex, GetFilePath());
		m_impl->shaderByteCodeBuffers[SP_Fragment] = m_impl->CompileShaderBytecode(SP_Fragment, GetFilePath());

		return Rendering::ShaderResource::Load();
	}

	bool
	ShaderResource::Unload()
	{
		if (!IsLoaded())
			return true;

		m_impl->shaderByteCodeBuffers.clear();

		return Rendering::ShaderResource::Unload();
	}

	bool
	ShaderResource::DeviceLoad(void* a_params)
	{
		OYL_PROFILE_FUNCTION();

		const ShaderDeviceLoadParams& params = *static_cast<ShaderDeviceLoadParams*>(a_params);

		auto vertexModule = m_impl->CompileShaderModule(params.device, SP_Vertex);
		auto fragmentModule = m_impl->CompileShaderModule(params.device, SP_Fragment);

		// We don't need the byte code on the CPU anymore
		m_impl->shaderByteCodeBuffers.clear();

		vk::PipelineShaderStageCreateInfo vertShaderStageCreateInfo {
			.stage = vk::ShaderStageFlagBits::eVertex,
			.module = vertexModule,
			.pName = "VertMain"
		};

		vk::PipelineShaderStageCreateInfo fragShaderStageCreateInfo {
			.stage = vk::ShaderStageFlagBits::eFragment,
			.module = fragmentModule,
			.pName = "FragMain"
		};

		vk::PipelineShaderStageCreateInfo shaderStages[] = {
			vertShaderStageCreateInfo,
			fragShaderStageCreateInfo
		};

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
				.stageCount = 2,
				.pStages = shaderStages,
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

		m_impl->pipeline = vk::raii::Pipeline(
			params.device,
			nullptr,
			pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>()
		);

		return Rendering::ShaderResource::DeviceLoad(a_params);
	}

	bool
	ShaderResource::DeviceUnload(void* a_params)
	{
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
		ShaderProfile a_profile
	)
	{
		OYL_PROFILE_FUNCTION();

		const auto& bytecode = shaderByteCodeBuffers[a_profile];

		// Create a Vulkan shader module from the compilation result
		vk::ShaderModuleCreateInfo shaderModuleCreateInfo {
			.codeSize = bytecode.size(),
			.pCode = (uint32*) bytecode.data(),
		};
		return vk::raii::ShaderModule(a_device, shaderModuleCreateInfo);
	}

	std::vector<byte>
	ShaderResource::Impl::CompileShaderBytecode(ShaderProfile a_profile, std::string_view a_filePath)
	{
		OYL_PROFILE_FUNCTION();

		OYL_ASSERT(dxcCreateInstanceFn);

		HRESULT hres;

		CComPtr<IDxcLibrary> library;
		hres = dxcCreateInstanceFn(CLSID_DxcLibrary, IID_PPV_ARGS(&library));
		if (FAILED(hres))
			throw std::runtime_error("Could not init DXC library");

		// Initialize DXC compiler
		CComPtr<IDxcCompiler3> compiler;
		hres = dxcCreateInstanceFn(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
		if (FAILED(hres))
			throw std::runtime_error("Could not init DXC Compiler");

		// Initialize DXC utility
		CComPtr<IDxcUtils> utils;
		hres = dxcCreateInstanceFn(CLSID_DxcUtils, IID_PPV_ARGS(&utils));
		if (FAILED(hres))
			throw std::runtime_error("Could not init DXC Utility");

		std::wstring filePath { a_filePath.begin(), a_filePath.end() };

		uint32 codePage = DXC_CP_ACP;
		CComPtr<IDxcBlobEncoding> sourceBlob;
		hres = utils->LoadFile(filePath.c_str(), &codePage, &sourceBlob);
		if (FAILED(hres))
			throw std::runtime_error("Could not load shader file");

		std::wstring_view entryPoint = EntryPointFromProfile(a_profile);
		std::wstring_view profileString = ProfileStringFromProfile(a_profile);

		// Configure the compiler arguments for compiling the HLSL shader to SPIR-V
		std::vector<LPCWSTR> arguments = {
			// Shader main entry point
			L"-E",
			entryPoint.data(),

			// Shader target profile
			L"-T",
			profileString.data(),

			// Compile to SPIRV
			L"-spirv",
			L"-fspv-target-env=vulkan1.3",
		};

		DxcBuffer buffer {
			.Ptr = sourceBlob->GetBufferPointer(),
			.Size = sourceBlob->GetBufferSize(),
			.Encoding = DXC_CP_ACP,
		};

		CComPtr<IDxcResult> result = nullptr;
		hres = compiler->Compile(
			&buffer,
			arguments.data(),
			uint32(arguments.size()),
			nullptr,
			IID_PPV_ARGS(&result)
		);

		if (SUCCEEDED(hres))
			result->GetStatus(&hres);

		// Output error if compilation failed
		if (FAILED(hres) && (result))
		{
			CComPtr<IDxcBlobEncoding> errorBlob;
			hres = result->GetErrorBuffer(&errorBlob);
			if (SUCCEEDED(hres) && errorBlob)
			{
				OYL_LOG_ERROR("Shader compilation failed :\n{}", (const char*) errorBlob->GetBufferPointer());
				throw std::runtime_error("Compilation failed");
			}
		}

		CComPtr<IDxcBlob> code;
		result->GetResult(&code);
		auto codeBuffer = (byte*) code->GetBufferPointer();

		// Get the bytecode as a vector
		std::vector<byte> byteCodeVector;
		byteCodeVector.insert(byteCodeVector.end(), &codeBuffer[0], &codeBuffer[code->GetBufferSize()]);

		return byteCodeVector;
	}
}
