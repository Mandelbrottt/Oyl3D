#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "VulkanDevice.h"
#include "VulkanPresentTarget.h"
#include "VulkanShaderCompiler.h"
#include "VulkanSwapChain.h"

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

			const Rendering::VulkanShaderCompiler&
			GetShaderCompiler() const override
			{
				return m_shaderCompiler;
			}

			Rendering::PresentTargetHandle
			CreatePresentTarget(const Rendering::PresentTarget::CreateParams& a_params) const override;

			Rendering::VulkanDevice&
			GetCurrentDevice() override
			{
				return m_device;
			}

			const Rendering::VulkanDevice&
			GetCurrentDevice() const override
			{
				return const_cast<VulkanRenderEngineInstance*>(this)->GetCurrentDevice();
			}

			Rendering::SwapChain&
			GetCurrentSwapChain() override
			{
				return m_swapChain;
			}

			const Rendering::SwapChain&
			GetCurrentSwapChain() const override
			{
				return const_cast<VulkanRenderEngineInstance*>(this)->GetCurrentSwapChain();
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

			Rendering::VulkanShaderCompiler m_shaderCompiler;

			Rendering::VulkanDevice m_device = nullptr;
			Rendering::VulkanSwapChain m_swapChain = nullptr;
			Rendering::VulkanPresentTarget m_presentTarget = nullptr;

			const IWindow* m_window;
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
