#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "Rendering/PresentTarget.h"

namespace Oyl::Rendering
{
	using VulkanPresentTargetId = OpaqueHandleConvertible<PresentTargetId, vk::SurfaceKHR>;

	class OYL_RENDERING_API VulkanPresentTarget : public PresentTarget
	{
	public:
		VulkanPresentTarget() = default;

		struct CreateParams
		{
			const IWindow& window;
			const vk::raii::Instance& vkInstance;
		};

		explicit
		VulkanPresentTarget(const CreateParams& a_params);

		DEFAULT_MOVE(VulkanPresentTarget);

		virtual
		~VulkanPresentTarget();

		void
		Destroy() override;

		bool
		IsValid() const override;

		const vk::raii::SurfaceKHR&
		GetVkSurface() const { return m_vkSurface; }

		VulkanPresentTargetId
		GetId() const { return *m_vkSurface; }

		operator VulkanPresentTargetId() const { return GetId(); }

	protected:
		PresentTargetId
		GetIdImpl() const override { return GetId(); }

	private:
		void
		CreateVkSurface(const vk::raii::Instance& a_vkInstance, const IWindow& a_window);

	private:
		vk::raii::SurfaceKHR m_vkSurface = nullptr;
	};

	using VulkanPresentTargetHandle = UniquePtrImplicitConvertible<VulkanPresentTarget>;
}
