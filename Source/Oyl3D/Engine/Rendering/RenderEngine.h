#pragma once

#include <Core/UniquePtr.h>
#include <Core/Math/Vector.h>

#include "Enums.h"

#include "Rendering/Device.h"
#include "Rendering/RenderContext.h"
#include "Rendering/ShaderCompiler.h"

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

			struct CreateParams
			{
				std::unique_ptr<Rendering::ShaderCompiler> shaderCompiler;
				std::unique_ptr<Rendering::RenderContext> renderContext;
			};

			explicit
			RenderEngineInstance(CreateParams a_params);

		public:
			virtual
			~RenderEngineInstance();

	#pragma region Instance State
			virtual
			ResourceManager*
			GetResourceManager() const;

			virtual
			const Rendering::ShaderCompiler*
			GetShaderCompiler() const;

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
			std::unique_ptr<ResourceManager> m_resourceManager;
			std::unique_ptr<Rendering::ShaderCompiler> m_shaderCompiler;
			std::unique_ptr<Rendering::Device> m_device;

			// TEMPORARY: Should the renderer own the render context?
			std::unique_ptr<Rendering::RenderContext> m_renderContext;
		};
	}

	class OYL_RENDERING_API RenderEngine
	{
		friend class RenderControlModule;
		friend class RenderDirectorModule;

	public:
		RenderEngine() = delete;

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
		CreateInstance(Rendering::GraphicsApi a_api);

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
