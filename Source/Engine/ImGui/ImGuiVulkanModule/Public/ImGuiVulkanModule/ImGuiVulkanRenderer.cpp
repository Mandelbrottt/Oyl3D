#include "ImGuiVulkanRenderer.h"

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
	ImGuiVulkanRenderer::Init(Renderer& a_renderer, IApplicationWindow& a_mainWindow)
	{
		OYL_PROFILE_FUNCTION();

		OYL_ASSERT(a_renderer.GetRenderContext().GetRenderingApi() == RenderingApi::Vulkan);

		m_mainWindow = &a_mainWindow;

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

		GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(m_mainWindow->GetNativeWindowHandle());

		auto& vkPhysicalDevice = vulkanDevice.GetVkPhysicalDevice();

		auto& vulkanRenderContext = static_cast<VulkanRenderContext&>(a_renderer.GetRenderContext());
		auto& vkInstance = vulkanRenderContext.GetVkInstance();
		auto& vulkanQueue = *vulkanDevice.GetCommandQueue(CommandQueueFlagBits::Graphics);

		auto* rendererWindowData = a_renderer.GetWindowData(*m_mainWindow);
		auto& swapChain = rendererWindowData->swapChain;

		m_renderPass = MakeUniquePtr<ImGuiVulkanRenderPass>(swapChain->GetSize());

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForVulkan(glfwWindow, true);
		ImGui_ImplGlfw_SetCallbacksChainForAllWindows(true);
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.ApiVersion = (uint32) vkInstance.getDispatcher()->getVkHeaderVersion();
		init_info.Instance = *vkInstance;
		init_info.PhysicalDevice = *vkPhysicalDevice;
		init_info.Device = *vkDevice;
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
	ImGuiVulkanRenderer::Update(Renderer& a_renderer)
	{
		OYL_PROFILE_FUNCTION();
		OYL_UNUSED(a_renderer);

		OYL_ASSERT(m_mainWindow);

		static bool show_demo_window = true;
		static bool show_another_window = false;
		static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		auto& io = ImGui::GetIO();

		std::lock_guard lock(m_renderMutex);

        // Start the Dear ImGui frame
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

		ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

		if (m_onUpdate)
			m_onUpdate();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }
	}

	void
	ImGuiVulkanRenderer::Render(Renderer& a_renderer, ICommandBuffer& a_commandBuffer)
	{
		OYL_PROFILE_FUNCTION();
		OYL_UNUSED(a_renderer, a_commandBuffer);

		std::lock_guard lock(m_renderMutex);

		m_renderPass->SetGameViewPortColorAttachment(m_gameViewPortImageHandle);
		m_renderPass->Execute(a_commandBuffer);

		auto& io = ImGui::GetIO();
        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

	}

	void
	ImGuiVulkanRenderer::Shutdown(Renderer& a_renderer)
	{
		OYL_PROFILE_FUNCTION();
		OYL_UNUSED(a_renderer);

		RenderEngine::Get().GetDevice().WaitUntilIdle();

		m_renderPass.Reset();
		m_gameViewPortImageHandle.Reset();

		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();

		m_vkDescriptorPool.clear();
	}

	void
	ImGuiVulkanRenderer::OnSwapChainRecreate(const ISwapChain& a_swapChain)
	{
        ImGui_ImplVulkan_SetMinImageCount(a_swapChain.GetMinImageCount());
		m_renderPass->OnViewPortResize(a_swapChain.GetSize());
	}
}
