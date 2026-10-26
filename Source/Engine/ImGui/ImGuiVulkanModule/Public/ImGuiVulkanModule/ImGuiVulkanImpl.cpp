#include "ImGuiVulkanImpl.h"

#include <vulkan/vulkan_raii.hpp>

#include <ImGui/backends/imgui_impl_glfw.h>
#include <ImGui/backends/imgui_impl_vulkan.h>

#include <Rendering/RenderEngine.h>
#include <Rendering/Renderer.h>
#include <Rendering/TestRenderPass.h>

#include <VulkanModule/VulkanDevice.h>
#include <VulkanModule/VulkanRenderContext.h>

namespace
{
	void
	CheckVkResult(VkResult a_err)
	{
		if (a_err == VK_SUCCESS)
			return;
		Oyl::LogError("[Vulkan] Error: VkResult = %d\n", (int32) a_err);
		OYL_ASSERT(a_err >= VK_SUCCESS);
	}
}

namespace Oyl
{
	void
	ImGuiVulkanImpl::Init(IImGuiRenderer& a_imGuiRenderer, IApplicationWindow& a_mainWindow)
	{
		OYL_PROFILE_FUNCTION();

		auto& renderer = a_imGuiRenderer.GetRenderer();

		OYL_ASSERT(renderer.GetRenderContext().GetRenderingApi() == RenderingApi::Vulkan);

		m_mainWindow = &a_mainWindow;
		m_imGuiRenderer = &a_imGuiRenderer;

		CreateDescriptorPools();
		InitImGuiVulkan();

		auto& swapChain = a_imGuiRenderer.GetRenderer().GetWindowData(a_mainWindow)->swapChain;
		swapChain->AddOnRecreateCallback({ this, &ImGuiVulkanImpl::OnSwapChainRecreate });
	}

	void
	ImGuiVulkanImpl::Update()
	{
		OYL_PROFILE_FUNCTION();

		// Start the Dear ImGui frame
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
	}

	void
	ImGuiVulkanImpl::Render(ICommandBuffer& a_commandBuffer)
	{
		OYL_PROFILE_FUNCTION();

		const auto& gameViewPortImageHandle = m_imGuiRenderer->GetGameViewPortImageHandle();
		m_renderPass->SetGameViewPortColorAttachment(gameViewPortImageHandle);
		m_renderPass->Execute(a_commandBuffer);
	}

	void
	ImGuiVulkanImpl::Shutdown()
	{
		OYL_PROFILE_FUNCTION();

		{
			auto* windowData = m_imGuiRenderer->GetRenderer().GetWindowData(*m_mainWindow);
			if (windowData && windowData->swapChain)
				windowData->swapChain->RemoveOnRecreateCallback({ this, &ImGuiVulkanImpl::OnSwapChainRecreate });
		}

		RenderEngine::Get().GetDevice().WaitUntilIdle();

		m_renderPass.Reset();

		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();

		m_vkDescriptorPool.clear();
	}

	void
	ImGuiVulkanImpl::CreateDescriptorPools()
	{
		// Create Descriptor Pool to be sent into ImGui
		vk::DescriptorPoolSize poolSizes[] = {
			{ vk::DescriptorType::eSampledImage, IMGUI_IMPL_VULKAN_MINIMUM_SAMPLED_IMAGE_POOL_SIZE },
			{ vk::DescriptorType::eSampler, IMGUI_IMPL_VULKAN_MINIMUM_SAMPLER_POOL_SIZE },
		};
		vk::DescriptorPoolCreateInfo createInfo {
			.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
			.maxSets = [&]() -> uint32
			{
				uint32 result = 0;
				for (auto& [type, descriptorCount] : poolSizes)
					result += descriptorCount;
				return result;
			}(),
			.poolSizeCount = (uint32) std::size(poolSizes),
			.pPoolSizes = poolSizes,
		};
		auto& vulkanDevice = static_cast<VulkanDevice&>(RenderEngine::Get().GetDevice());
		auto& vkDevice = vulkanDevice.GetVkDevice();
		m_vkDescriptorPool = vk::raii::DescriptorPool(vkDevice, createInfo);
	}

	void
	ImGuiVulkanImpl::InitImGuiVulkan()
	{
		auto& renderer = m_imGuiRenderer->GetRenderer();

		auto& vulkanDevice = static_cast<VulkanDevice&>(RenderEngine::Get().GetDevice());
		GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(m_mainWindow->GetNativeWindowHandle());

		auto& vkPhysicalDevice = vulkanDevice.GetVkPhysicalDevice();

		auto& vulkanRenderContext = static_cast<VulkanRenderContext&>(renderer.GetRenderContext());
		auto& vkInstance = vulkanRenderContext.GetVkInstance();
		auto& vulkanQueue = *vulkanDevice.GetCommandQueue(CommandQueueFlagBits::Graphics);

		auto* rendererWindowData = renderer.GetWindowData(*m_mainWindow);
		auto& swapChain = rendererWindowData->swapChain;

		m_renderPass = MakeUniquePtr<ImGuiVulkanRenderPass>(swapChain->GetSize());

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForVulkan(glfwWindow, true);
		ImGui_ImplGlfw_SetCallbacksChainForAllWindows(true);
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.ApiVersion = (uint32) vkInstance.getDispatcher()->getVkHeaderVersion();
		init_info.Instance = *vkInstance;
		init_info.PhysicalDevice = *vkPhysicalDevice;
		init_info.Device = *vulkanDevice.GetVkDevice();
		init_info.QueueFamily = vulkanQueue.GetVkQueueFamilyIndex();
		init_info.Queue = *vulkanQueue.GetVkQueue();

		// Dynamic Rendering
		init_info.UseDynamicRendering = true;
		vk::Format colorAttachmentFormat = ToVkEnum(m_renderPass->GetColorAttachment()->GetFormat());
		init_info.PipelineInfoMain.PipelineRenderingCreateInfo = vk::PipelineRenderingCreateInfo {
			.colorAttachmentCount = 1,
			.pColorAttachmentFormats = &colorAttachmentFormat,
		};
		// Dynamic Rendering

		init_info.DescriptorPool = *m_vkDescriptorPool;
		init_info.MinImageCount = swapChain->GetMinImageCount();
		init_info.ImageCount = swapChain->GetImageCount();
		init_info.Allocator = nullptr;
		init_info.PipelineInfoMain.RenderPass = nullptr;
		init_info.PipelineInfoMain.Subpass = 0;
		init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		init_info.CheckVkResultFn = CheckVkResult;
		ImGui_ImplVulkan_Init(&init_info);
	}

	void
	ImGuiVulkanImpl::OnSwapChainRecreate(const ISwapChain& a_swapChain)
	{
		ImGui_ImplVulkan_SetMinImageCount(a_swapChain.GetMinImageCount());
		m_renderPass->OnViewPortResize(a_swapChain.GetSize());
	}
}
