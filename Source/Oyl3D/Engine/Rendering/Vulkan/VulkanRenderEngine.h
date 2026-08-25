#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "VulkanDevice.h"
#include "VulkanShaderCompiler.h"

#include "Rendering/RenderEngine.h"

namespace Oyl
{
	namespace Internal
	{
		class OYL_RENDERING_API VulkanRenderEngineInstance : public RenderEngineInstance
		{
		public:
			struct CreateParams
			{
				const IWindow* window;
			};

			explicit
			VulkanRenderEngineInstance(CreateParams a_params);

			virtual
			~VulkanRenderEngineInstance();

			const Rendering::VulkanShaderCompiler*
			GetShaderCompiler() const override
			{
				const auto* abstract = RenderEngineInstance::GetShaderCompiler();
				return static_cast<const Rendering::VulkanShaderCompiler*>(abstract);
			}

			Rendering::PresentTargetHandle
			CreatePresentTarget(const Rendering::PresentTarget::CreateParams& a_params) const override;

			const Rendering::VulkanDevice*
			GetCurrentDevice() const override
			{
				const auto* abstract = RenderEngineInstance::GetCurrentDevice();
				return static_cast<const Rendering::VulkanDevice*>(abstract);
			}

			const vk::raii::Context&
			GetVkContext() const
			{
				return m_vkContext;
			}

			const vk::raii::Instance&
			GetVkInstance() const
			{
				return m_vkInstance;
			}

		private:
			void
			CreateVkInstance();
			void
			CreateVkDebugMessenger();

		private:
			vk::raii::Context m_vkContext;
			vk::raii::Instance m_vkInstance = nullptr;
			vk::raii::DebugUtilsMessengerEXT m_debugMessenger = nullptr;
		};
	}

	class OYL_RENDERING_API VulkanRenderEngine : public RenderEngine
	{
	public:
		static
		const vk::raii::Context&
		GetVkContext()
		{
			return CurrentInstance()->GetVkContext();
		}

		static
		const vk::raii::Instance&
		GetVkInstance()
		{
			return CurrentInstance()->GetVkInstance();
		}

	protected:
		static
		Internal::VulkanRenderEngineInstance*
		CurrentInstance()
		{
			auto instance = dynamic_cast<Internal::VulkanRenderEngineInstance*>(RenderEngine::CurrentInstance());
			OYL_ASSERT(instance);
			return instance;
		}
	};
}
