#include "VulkanRenderContext.h"

#include <vulkan/vulkan_raii.hpp>

#include "VulkanDevice.h"
#include "VulkanRenderQueue.h"
#include "VulkanShaderResource.h"
#include "VulkanSwapChain.h"
#include "VulkanVertexBuffer.h"

#include "Rendering/RenderEngine.h"
#include "Rendering/Glfw/GlfwWindow.h"
#include "Rendering/Vulkan/VulkanCommandBuffer.h"
#include "Rendering/Vulkan/VulkanCommandPool.h"

static const std::vector REQUIRED_DEVICE_EXTENSION {
	vk::KHRSwapchainExtensionName,
};

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

namespace Oyl::Rendering::Vulkan
{
	struct RenderContext::Impl
	{
		const IWindow* window;

		Device device;
		RenderQueue graphicsQueue;
		SwapChain swapChain;

		// TEMPORARY:
		Shader shader;

		// TEMPORARY:
		VertexBuffer vertexBuffer;

		CommandPool commandPool;
		std::vector<CommandBuffer> commandBuffers;

		std::vector<Semaphore> presentCompleteSemaphores;
		std::vector<Semaphore> renderFinishedSemaphores;
		std::vector<Fence> inFlightFences;

		uint32 frameIndex = 0;

		void
		CreateSyncObjects();
		void
		RecordCommandBuffer();
		void
		DrawFrame();
	};

	RenderContext::RenderContext() noexcept
		: m_impl(nullptr) {}

	RenderContext::RenderContext(const CreateParams& a_params) noexcept
		: m_impl(nullptr)
	{
		Init(a_params);
	}

	RenderContext::RenderContext(RenderContext&& a_other) noexcept
		: m_impl(nullptr)
	{
		*this = std::move(a_other);
	}

	RenderContext&
	RenderContext::operator=(RenderContext&& a_other) noexcept
	{
		if (this != &a_other)
		{
			m_impl = std::move(a_other.m_impl);
		}
		return *this;
	}

	RenderContext::~RenderContext()
	{
		RenderContext::Destroy();
	}

	void
	RenderContext::Init(const CreateParams& a_params)
	{
		OYL_PROFILE_FUNCTION();

		if (!m_impl)
			m_impl = std::make_unique<Impl>();

		m_impl->window = a_params.window;

		m_impl->device = Device(
			{
				.window = *m_impl->window,
				.ppRequiredDeviceExtensionsData = REQUIRED_DEVICE_EXTENSION.data(),
				.requiredDeviceExtensionsLength = REQUIRED_DEVICE_EXTENSION.size()
			}
		);

		m_impl->graphicsQueue = RenderQueue(
			{
				.device = m_impl->device,
				.queueFamilyIndex = m_impl->device.GetVkGraphicsQueueFamilyIndex()
			}
		);

		m_impl->swapChain = SwapChain(
			{
				.window = *m_impl->window,
				.device = m_impl->device,
			}
		);

		m_impl->commandPool = CommandPool({ .device = m_impl->device });

		m_impl->commandBuffers.reserve(MAX_FRAMES_IN_FLIGHT);
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_impl->commandBuffers.emplace_back(
				CommandBuffer::CreateParams {
					.device = m_impl->device,
					.commandPool = m_impl->commandPool,
				}
			);
		}

		m_impl->CreateSyncObjects();
	}

	void
	RenderContext::Update()
	{
		OYL_PROFILE_FUNCTION();

		if (!m_impl->window || !m_impl->window->IsValid())
			return;

		if (!m_impl->shader)
		{
			//m_impl->shader = RenderEngine::CreateShader(
			//	{
			//		.language = SL_Hlsl,
			//		.source = ShaderOptions::SO_File,
			//		.filepath = "G:/dev/Oyl3D/Oyl3D/Source/Oyl3D/Engine/Rendering/Shaders/shader.hlsl"
			//	}
			//);
		}

		if (m_impl->shader->IsDirty())
		{
			m_impl->shader->Load();
		}

		//if (m_impl->shader->IsDeviceDirty())
		//{
		//	m_impl->shader->DeviceLoad(m_impl->device);
		//}

		if (!m_impl->vertexBuffer)
		{
			const std::vector vertices {
				Vertex { Vector2f { -0.5f, -0.5f }, Vector3f { 1.0f, 0.0f, 0.0f } },
				Vertex { Vector2f { 0.5f, -0.5f }, Vector3f { 0.0f, 1.0f, 0.0f } },
				Vertex { Vector2f { 0.5f, 0.5f }, Vector3f { 0.0f, 0.0f, 1.0f } },
				Vertex { Vector2f { -0.5f, 0.5f }, Vector3f { 1.0f, 1.0f, 1.0f } },
			};

			const std::vector<uint16> indices { 0, 1, 2, 2, 3, 0 };

			auto verticesBuffer = reinterpret_cast<const byte*>(vertices.data());
			auto indicesBuffer = reinterpret_cast<const byte*>(indices.data());

			//m_impl->vertexBuffer = RenderEngine::CreateVertexBuffer(
			//	{
			//		.vertexData = verticesBuffer,
			//		.vertexLength = vertices.size() * sizeof(decltype(vertices)::value_type),
			//		.vertexStride = sizeof(decltype(vertices)::value_type),
			//		.indexData = indicesBuffer,
			//		.indexLength = indices.size() * sizeof(decltype(indices)::value_type),
			//	}
			//);

			m_impl->vertexBuffer = VertexBuffer({
				.device = m_impl->device,
				.queue = m_impl->graphicsQueue,
				.vertexData = verticesBuffer,
				.vertexLength = vertices.size() * sizeof(decltype(vertices)::value_type),
				.vertexStride = sizeof(decltype(vertices)::value_type),
				.indexData = indicesBuffer,
				.indexLength = indices.size() * sizeof(decltype(indices)::value_type)
			});
		}

		//if (m_impl->vertexBuffer->IsDirty())
		//{
		//	m_impl->vertexBuffer->Load();
		//}

		//if (m_impl->vertexBuffer->IsDeviceDirty())
		//{
		//	m_impl->vertexBuffer->DeviceLoad(m_impl->device);
		//}

		m_impl->DrawFrame();
	}

	void
	RenderContext::Destroy()
	{
		OYL_PROFILE_FUNCTION();

		if (!m_impl)
			return;

		m_impl->device.GetVkDevice().waitIdle();

		m_impl->renderFinishedSemaphores.clear();
		m_impl->presentCompleteSemaphores.clear();
		m_impl->inFlightFences.clear();

		m_impl->commandBuffers.clear();
		m_impl->commandPool.Destroy();

		m_impl->vertexBuffer.Destroy();

		//m_impl->vertexBuffer->DeviceUnload();
		//m_impl->vertexBuffer->Unload();

		//m_impl->shader->DeviceUnload();
		//m_impl->shader->Unload();

		m_impl->swapChain.Destroy();
		m_impl->device.Destroy();

		*m_impl = {};
	}

	void
	RenderContext::Resize(Vector2i /*a_size*/)
	{
		// No need to pass in a_size - we get the size from the window directly
		m_impl->swapChain.Recreate();
	}

	const Device*
	RenderContext::GetDevice() const
	{
		return &m_impl->device;
	}

	const SwapChain*
	RenderContext::GetSwapChain() const
	{
		return &m_impl->swapChain;
	}

	void
	RenderContext::Impl::CreateSyncObjects()
	{
		OYL_PROFILE_FUNCTION();

		OYL_ASSERT(renderFinishedSemaphores.empty() && presentCompleteSemaphores.empty() && inFlightFences.empty());

		for (size_t i = 0; i < swapChain.GetVkImages().size(); i++)
		{
			renderFinishedSemaphores.emplace_back(device);
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			presentCompleteSemaphores.emplace_back(device);
			inFlightFences.emplace_back(device);
		}
	}

	void
	RenderContext::Impl::RecordCommandBuffer()
	{
		OYL_PROFILE_FUNCTION();

		auto& commandBuffer = commandBuffers[frameIndex];

		commandBuffer.Begin();
		commandBuffer.BeginRendering(swapChain);

		Vector2u extent = Vector2u(
			swapChain.GetVkExtent().width,
			swapChain.GetVkExtent().height
		);
		commandBuffer.SetViewport(Vector2i::Zero(), extent);
		commandBuffer.SetScissor(Vector2i::Zero(), extent);

		commandBuffer.BindShader(*shader);
		commandBuffer.BindVertexBuffer(vertexBuffer);

		commandBuffer.EndRendering(swapChain);
		commandBuffer.End();
	}

	void
	RenderContext::Impl::DrawFrame()
	{
		OYL_PROFILE_FUNCTION();

		if (!swapChain)
			return;

		auto& drawFence = inFlightFences[frameIndex];
		auto& presentCompleteSemaphore = presentCompleteSemaphores[frameIndex];

		drawFence.Wait();

		bool success = swapChain.AcquireNextImage(presentCompleteSemaphore, nullptr);
		if (!success)
		{
			swapChain.Recreate();
			return;
		}

		auto imageIndex = swapChain.GetCurrentImageIndex();

		// Only reset fences if we are going to submit work to the GPU
		drawFence.Reset();

		RecordCommandBuffer();

		auto& renderFinishedSemaphore = renderFinishedSemaphores[imageIndex];

		auto& commandBuffer = commandBuffers[frameIndex];

		graphicsQueue.Submit(RenderQueue::SubmitParams {
			.commandBuffer = commandBuffer,
			.waitSemaphore = presentCompleteSemaphore,
			.signalSemaphore = renderFinishedSemaphore,
			.fence = drawFence
		});

		bool result = graphicsQueue.Present(RenderQueue::PresentParams {
			.waitSemaphore = renderFinishedSemaphore,
			.swapChain = swapChain
		});

		if (!result)
			swapChain.Recreate();

		frameIndex = (frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
	}
}