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
	enum class ShaderProfile
	{
		Vertex,
		Fragment,
	};

	std::wstring_view
	EntryPointFromProfile(ShaderProfile a_profile)
	{
		switch (a_profile)
		{
			case ShaderProfile::Vertex:
				return L"VertMain";
			case ShaderProfile::Fragment:
				return L"FragMain";
		}

		return L"";
	}

	std::wstring_view
	ProfileStringFromProfile(ShaderProfile a_profile)
	{
		switch (a_profile)
		{
			case ShaderProfile::Vertex:
				return L"vs_6_4";
			case ShaderProfile::Fragment:
				return L"ps_6_4"; // HLSL uses Pixel Shader instead of Fragment Shader
		}

		return L"";
	}

	struct ShaderResource::Impl
	{
		vk::raii::Pipeline pipeline = nullptr;

		// For compiling hlsl into spir-v
		SharedLibrary dxcCompilerLibrary;
		decltype(DxcCreateInstance)* dxcCreateInstanceFn;

		vk::raii::ShaderModule
		CompileShaderModule(
			const vk::raii::Device& a_device,
			ShaderProfile a_profile,
			std::string_view a_filePath
		);
	};

	ShaderResource::ShaderResource()
		: m_impl(std::make_unique<Impl>())
	{
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
		return Rendering::ShaderResource::Load();
	}

	bool
	ShaderResource::Unload()
	{
		return Rendering::ShaderResource::Unload();
	}

	const vk::raii::Pipeline&
	ShaderResource::GetPipeline() const
	{
		return m_impl->pipeline;
	}

	bool
	ShaderResource::Compile(const ShaderCompileInput& a_input)
	{
		OYL_PROFILE_FUNCTION();

		auto vertexModule = m_impl->CompileShaderModule(a_input.device, ShaderProfile::Vertex, GetFilePath());
		auto fragmentModule = m_impl->CompileShaderModule(a_input.device, ShaderProfile::Fragment, GetFilePath());

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

		vk::PipelineVertexInputStateCreateInfo vertexInputInfo;

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

		auto pipelineLayout = vk::raii::PipelineLayout(a_input.device, pipelineLayoutInfo);

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
				.pColorAttachmentFormats = &a_input.format
			}
		};

		m_impl->pipeline = vk::raii::Pipeline(
			a_input.device,
			nullptr,
			pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>()
		);

		return Rendering::ShaderResource::Compile();
	}

	vk::raii::ShaderModule
	ShaderResource::Impl::CompileShaderModule(
		const vk::raii::Device& a_device,
		ShaderProfile a_profile,
		std::string_view a_filePath
	)
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

		// Create a Vulkan shader module from the compilation result
		vk::ShaderModuleCreateInfo shaderModuleCreateInfo {
			.codeSize = code->GetBufferSize(),
			.pCode = (uint32*) code->GetBufferPointer(),
		};
		return vk::raii::ShaderModule(a_device, shaderModuleCreateInfo);
	}
}
