#pragma once

#include <Core/UniquePtr.h>
#include <Core/Math/Vector.h>

#include "Enums.h"

#include "Rendering/Device.h"
#include "Rendering/RenderContext.h"
#include "Rendering/ShaderCompiler.h"
#include "Rendering/PresentTarget.h"

namespace Oyl::Internal
{
	class ResourceManager;
}

namespace Oyl
{
	namespace Internal
	{
		class OYL_RENDERING_API RenderEngineInstance
		{
		protected:
			RenderEngineInstance();

		public:
			virtual
			~RenderEngineInstance();

	#pragma region Instance State
			virtual
			ResourceManager*
			GetResourceManager();

			virtual
			const Rendering::ShaderCompiler*
			GetShaderCompiler() const;

			virtual
			Rendering::PresentTargetHandle
			CreatePresentTarget(const Rendering::PresentTarget::CreateParams& a_params) const = 0;

			virtual
			const Rendering::Device*
			GetCurrentDevice() const;

			virtual
			Vector2u
			GetCurrentViewPortSize() const;

			virtual
			Rendering::RenderContext*
			GetRenderContext();

			virtual
			const Rendering::RenderContext*
			GetRenderContext() const;
	#pragma endregion Instance State

		protected:
			UniquePtr<ResourceManager> m_resourceManager;
			UniquePtr<Rendering::ShaderCompiler> m_shaderCompiler;

			// TEMPORARY: Should the renderer own the render context?
			UniquePtr<Rendering::RenderContext> m_renderContext;
		};
	}

	class OYL_RENDERING_API RenderEngine
	{
		friend class RenderControlModule;
		friend class RenderDirectorModule;

	public:
		RenderEngine() = delete;

		struct CreateParams
		{
			const IWindow* window;
		};

		static
		Rendering::PresentTargetHandle
		CreatePresentTarget(const Rendering::PresentTarget::CreateParams& a_params)
		{
			return s_instance->CreatePresentTarget(a_params);
		}

		static
		const Rendering::Device*
		GetCurrentDevice()
		{
			return s_instance->GetCurrentDevice();
		}

		static
		Vector2u
		GetCurrentViewPortSize()
		{
			return s_instance->GetCurrentViewPortSize();
		}

		static
		const Rendering::ShaderCompiler*
		GetShaderCompiler()
		{
			return s_instance->GetShaderCompiler();
		}

	protected:
		static
		UniquePtr<Internal::RenderEngineInstance>
		CreateInstance(Rendering::GraphicsApi a_api, const CreateParams& a_params);

		static
		Internal::RenderEngineInstance*
		CurrentInstance()
		{
			return s_instance;
		}

		static
		void
		SetCurrentInstance(Internal::RenderEngineInstance* a_instance)
		{
			s_instance = a_instance;
		}

	private:
		static Internal::RenderEngineInstance* s_instance;
	};
}
