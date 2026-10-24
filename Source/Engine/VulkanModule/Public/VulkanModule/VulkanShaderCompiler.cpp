#include "VulkanShaderCompiler.h"

#include <Core/Containers/ArrayMap.h>
#include <Core/Platform/SharedLibrary.h>

#if defined(OYL_WINDOWS)
#include <atlcomcli.h>
#include <d3d12shader.h>
#include <Windows.h>

#include <dxc/dxcapi.h>

namespace
{
	class HResultHandler
	{
	public:
		HResultHandler() = default;

		HResultHandler(HRESULT a_hres)
		{
			*this = a_hres;
		}

		HResultHandler&
		operator =(HRESULT a_hres)
		{
			if (FAILED(a_hres))
				throw std::runtime_error("Bad HRESULT!");

			m_lastResult = a_hres;

			return *this;
		}

		operator HRESULT() const
		{
			return GetLastResult();
		}

		HRESULT
		GetLastResult() const
		{
			return m_lastResult;
		}

	private:
		HRESULT m_lastResult = 0;
	};
}
#endif

namespace Oyl
{
#if defined(OYL_WINDOWS)
	struct VulkanShaderCompiler::Impl
	{
		// For compiling hlsl into spir-v
		Platform::SharedLibraryHandle* dxcCompilerSharedLibrary;
		decltype(DxcCreateInstance)* dxcCreateInstanceFn;

		CComPtr<IDxcCompiler3> dxcCompiler;
		CComPtr<IDxcUtils> dxcUtils;

		Array<ShaderProfile>
		GetShaderProfilesInSource(StringView a_source) const;

		ShaderStage
		CompileHlslShaderStage(ShaderProfile a_profile, StringView a_source, StringView a_sourceName);
	};

	VulkanShaderCompiler::VulkanShaderCompiler()
	{
		OYL_PROFILE_FUNCTION();

		m_impl = MakeUniquePtr<Impl>();

		StringView dxCompilerLibName =
		#if defined(OYL_DEBUG)
			"dxcompilerd.dll";
		#else
			"dxcompiler.dll";
		#endif
		m_impl->dxcCompilerSharedLibrary = Platform::LoadSharedLibrary(dxCompilerLibName);
		OYL_ASSERT(m_impl->dxcCompilerSharedLibrary);

		m_impl->dxcCreateInstanceFn = (decltype(DxcCreateInstance)*) GetSymbolFromSharedLibrary(
			m_impl->dxcCompilerSharedLibrary,
			"DxcCreateInstance");
		OYL_ASSERT(m_impl->dxcCreateInstanceFn);

		HResultHandler hres;

		// Initialize DXC compiler
		hres = m_impl->dxcCreateInstanceFn(CLSID_DxcCompiler, IID_PPV_ARGS(&m_impl->dxcCompiler));

		// Initialize DXC utility
		hres = m_impl->dxcCreateInstanceFn(CLSID_DxcUtils, IID_PPV_ARGS(&m_impl->dxcUtils));
	}

	VulkanShaderCompiler::~VulkanShaderCompiler()
	{
		m_impl->dxcUtils.Release();
		m_impl->dxcCompiler.Release();

		if (m_impl->dxcCompilerSharedLibrary)
			FreeSharedLibrary(m_impl->dxcCompilerSharedLibrary);
	}

	VulkanShaderCompiler::VulkanShaderCompiler(const VulkanShaderCompiler& a_other)
	{
		m_impl = MakeUniquePtr<Impl>();

		*this = a_other;
	}

	VulkanShaderCompiler&
	VulkanShaderCompiler::operator=(const VulkanShaderCompiler& a_other)
	{
		if (this == &a_other)
			return *this;

		*m_impl = *a_other.m_impl;
		return *this;
	}

	VulkanShaderCompiler::VulkanShaderCompiler(VulkanShaderCompiler&& a_other) noexcept
	{
		*this = std::move(a_other);
	}

	VulkanShaderCompiler&
	VulkanShaderCompiler::operator=(VulkanShaderCompiler&& a_other) noexcept
	{
		if (this == &a_other)
			return *this;

		std::swap(m_impl, a_other.m_impl);

		return *this;
	}

	bool
	VulkanShaderCompiler::CompileHlsl(StringView a_source, ShaderCompileResult* a_outShader) const
	{
		OYL_PROFILE_FUNCTION();

		if (!a_outShader)
			return false;

		auto shaderProfilesInSource = m_impl->GetShaderProfilesInSource(a_source);
		std::vector<ShaderStage> stages;
		for (auto profile : shaderProfilesInSource)
		{
			auto stage = m_impl->CompileHlslShaderStage(profile, a_source, a_outShader->GetFilePath());
			stages.emplace_back(std::move(stage));
		}

		// Temp outShader to store source code from CompileHlslFromSource
		auto shaderCompileResultParams = ShaderCompileResult::Params {
			.filePath = a_outShader->GetFilePath(),
			.sourceCode = a_source,
			.stages = std::move(stages)
		};

		*a_outShader = ShaderCompileResult(shaderCompileResultParams);
		return true;
	}

	Array<ShaderProfile>
	VulkanShaderCompiler::Impl::GetShaderProfilesInSource(StringView a_source) const
	{
		OYL_PROFILE_FUNCTION();

		HResultHandler hres;

		// Copy the source code into an encoded blob
		CComPtr<IDxcBlobEncoding> blobEncoding;
		hres = dxcUtils->CreateBlob(a_source.data(), (uint32) a_source.length(), DXC_CP_ACP, &blobEncoding);
		if (FAILED(hres))
			throw std::runtime_error("Could not init Blob Encoding");

		// Set up the buffer descriptor
		DxcBuffer buffer {
			.Ptr = blobEncoding->GetBufferPointer(),
			.Size = blobEncoding->GetBufferSize(),
			.Encoding = 0,
		};

		std::vector<LPCWSTR> args = {
			// Strip debug and reflect from object
			// We will query reflect info separately
			L"-Qstrip_debug",
			L"-Qstrip_reflect",
			L"-Qstrip_priv",
			L"-Qstrip_rootsignature",

			//L"-default-linkage",
			//L"external",

			//DXC_ARG_WARNINGS_ARE_ERRORS,
			DXC_ARG_SKIP_OPTIMIZATIONS,
			DXC_ARG_SKIP_VALIDATION,
		};
		std::vector<DxcDefine> defines {

		};

		CComPtr<IDxcCompilerArgs> arguments;
		hres = dxcUtils->BuildArguments(
			nullptr,
			nullptr,
			L"lib_6_6", // Compile as a library to make all shader entry points visible
			args.data(),
			(uint32) args.size(),
			defines.data(),
			(uint32) defines.size(),
			&arguments
		);

		// Compile hlsl source
		CComPtr<IDxcResult> compileResult = nullptr;
		hres = dxcCompiler->Compile(
			&buffer,
			arguments->GetArguments(),
			arguments->GetCount(),
			nullptr,
			IID_PPV_ARGS(&compileResult)
		);

		CComPtr<IDxcBlobUtf8> errors;
		hres = compileResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr);
		if (errors && errors->GetStringLength() > 0)
		{
			LogError("Shader compilation failed :\n{}", (char*) errors->GetBufferPointer());
			throw std::runtime_error("Compilation failed");
		}

		ArrayMap<D3D12_SHADER_VERSION_TYPE, ShaderProfile> shaderTypeMap;
		shaderTypeMap[D3D12_SHVER_VERTEX_SHADER] = ShaderProfile::Vertex;
		shaderTypeMap[D3D12_SHVER_PIXEL_SHADER] = ShaderProfile::Fragment;

		CComPtr<IDxcBlob> reflectionData;
		hres = compileResult->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(&reflectionData), nullptr);
		DxcBuffer reflectionBuffer {
			.Ptr = reflectionData->GetBufferPointer(),
			.Size = reflectionData->GetBufferSize(),
			.Encoding = 0
		};

		CComPtr<ID3D12LibraryReflection> libraryReflection;
		hres = dxcUtils->CreateReflection(&reflectionBuffer, IID_PPV_ARGS(&libraryReflection));
		D3D12_LIBRARY_DESC libraryDesc;
		hres = libraryReflection->GetDesc(&libraryDesc);

		std::vector<ShaderProfile> shaderProfiles;
		shaderProfiles.reserve(libraryDesc.FunctionCount);

		for (uint32 i = 0; i < libraryDesc.FunctionCount; i++)
		{
			auto* functionReflection = libraryReflection->GetFunctionByIndex(i);
			D3D12_FUNCTION_DESC functionDesc;
			hres = functionReflection->GetDesc(&functionDesc);

			auto shaderType = (D3D12_SHADER_VERSION_TYPE) D3D12_SHVER_GET_TYPE(functionDesc.Version);

			// TODO: Don't Hardcode version numbers
			bool isValid = D3D12_SHVER_GET_MAJOR(functionDesc.Version) == 6
			               && D3D12_SHVER_GET_MINOR(functionDesc.Version) == 6;

			if (!isValid)
				continue;

			if (!shaderTypeMap.ContainsKey(shaderType))
				continue;

			auto shaderProfile = shaderTypeMap.At(shaderType);
			shaderProfiles.push_back(shaderProfile);
		}

		return shaderProfiles;
	}

	static
	StringView
	HlslEntryPointFromProfile(ShaderProfile a_profile)
	{
		switch (a_profile)
		{
			case ShaderProfile::Vertex:
				return "VertMain";
			case ShaderProfile::Geometry:
				return "GeomMain";
			case ShaderProfile::Fragment:
				return "FragMain";
			case ShaderProfile::None: break;
		}

		return "";
	}

	static
	StringView
	HlslProfileStringFromProfile(ShaderProfile a_profile)
	{
		switch (a_profile)
		{
			case ShaderProfile::Vertex:
				return "vs_6_6";
			case ShaderProfile::Geometry:
				return "gs_6_6";
			case ShaderProfile::Fragment:
				return "ps_6_6"; // HLSL uses Pixel Shader instead of Fragment Shader
			case ShaderProfile::None: break;
		}

		return "";
	}

	ShaderStage
	VulkanShaderCompiler::Impl::CompileHlslShaderStage(ShaderProfile a_profile, StringView a_source, StringView a_sourceName)
	{
		OYL_PROFILE_FUNCTION();

		HResultHandler hres;

		// Copy the source code into an encoded blob
		CComPtr<IDxcBlobEncoding> blobEncoding;
		hres = dxcUtils->CreateBlob(a_source.data(), (uint32) a_source.length(), DXC_CP_ACP, &blobEncoding);
		if (FAILED(hres))
			throw std::runtime_error("Could not init Blob Encoding");

		// Set up the buffer descriptor
		DxcBuffer buffer {
			.Ptr = blobEncoding->GetBufferPointer(),
			.Size = blobEncoding->GetBufferSize(),
			.Encoding = 0,
		};

		StringView entryPoint = HlslEntryPointFromProfile(a_profile);
		StringView profileString = HlslProfileStringFromProfile(a_profile);

		// Configure the compiler arguments for compiling the HLSL shader to SPIR-V
		std::vector<LPCWSTR> args = {
			// Compile to SPIRV
			L"-spirv",
			L"-fspv-target-env=vulkan1.3",

			DXC_ARG_WARNINGS_ARE_ERRORS,
		};
		std::vector<DxcDefine> defines {

		};

		std::wstring sourceName(a_sourceName.begin(), a_sourceName.end());

		CComPtr<IDxcCompilerArgs> arguments;
		hres = dxcUtils->BuildArguments(
			sourceName.c_str(),
			std::wstring(entryPoint.begin(), entryPoint.end()).c_str(),
			std::wstring(profileString.begin(), profileString.end()).c_str(),
			args.data(),
			(uint32) args.size(),
			defines.data(),
			(uint32) defines.size(),
			&arguments
		);

		// Compile hlsl source
		CComPtr<IDxcResult> compileResult = nullptr;
		hres = dxcCompiler->Compile(
			&buffer,
			arguments->GetArguments(),
			arguments->GetCount(),
			nullptr,
			IID_PPV_ARGS(&compileResult)
		);

		CComPtr<IDxcBlobUtf8> errors;
		hres = compileResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr);
		if (errors && errors->GetStringLength() > 0)
		{
			LogError("Shader compilation failed :\n{}", (char*) errors->GetBufferPointer());
			throw std::runtime_error("Compilation failed");
		}

		CComPtr<IDxcBlob> code;
		hres = compileResult->GetResult(&code);
		auto codeBuffer = (byte*) code->GetBufferPointer();

		auto params = ShaderStageCreateParams {
			.shaderProfile = a_profile,
			.buffer = codeBuffer,
			.bufferLength = code->GetBufferSize(),
			.entryPoint = std::string(entryPoint)
		};
		return ShaderStage(params);
	}
#else
	struct ShaderCompiler::Impl
	{
	};

	ShaderCompiler::ShaderCompiler()
	{
		m_impl = std::make_unique<Impl>();

		TODO_IMPLEMENT_ME()
	}

	ShaderCompiler::~ShaderCompiler() {}

	CompiledShader
	ShaderCompiler::CompileFromFile(StringView a_filePath) const
	{
		TODO_IMPLEMENT_ME()
	}
#endif
}
