#pragma once

#include <vulkan/vulkan_raii.hpp>

#include <Rendering/Hardware/HardwarePresentTarget.h>

namespace Oyl
{
	using VulkanPresentTargetId = IConvertibleRenderDeviceObjectId<HardwarePresentTargetId, vk::SurfaceKHR>;

	struct VulkanPresentTargetCreateParams
	{
		const vk::raii::Instance* vkInstance;
		const IApplicationWindow* window;
	};

	class VULKANMODULE_EXPORT VulkanPresentTarget : public IHardwarePresentTarget
	{
	public:
		VulkanPresentTarget(nullptr_t) {}

		explicit
		VulkanPresentTarget(const VulkanPresentTargetCreateParams& a_params);

		DEFAULT_MOVE(VulkanPresentTarget);

		virtual
		~VulkanPresentTarget();

		void
		Destroy() override;

		bool
		IsValid() const override { return *m_vkSurface; }

		const vk::raii::SurfaceKHR&
		GetVkSurface() const { return m_vkSurface; }

		VulkanPresentTargetId
		GetId() const { return *m_vkSurface; }

		operator VulkanPresentTargetId() const { return GetId(); }

	protected:
		HardwarePresentTargetId
		GetIdImpl() const override { return GetId(); }

	private:
		void
		CreateVkSurface(const vk::raii::Instance& a_vkInstance, const IApplicationWindow& a_window);

	private:
		vk::raii::SurfaceKHR m_vkSurface = nullptr;
	};
};
