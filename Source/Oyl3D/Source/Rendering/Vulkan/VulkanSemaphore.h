#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "Rendering/Semaphore.h"

namespace Oyl::Rendering
{
	class VulkanDevice;

	using VulkanSemaphoreId = OpaqueHandleConvertible<SemaphoreId, vk::Semaphore>;

	class OYL_RENDERING_API VulkanSemaphore : public Semaphore
	{
	public:
		VulkanSemaphore(nullptr_t) {}

		VulkanSemaphore(const VulkanDevice& a_device);

		VulkanSemaphore(VulkanSemaphore&& a_other) noexcept;
		VulkanSemaphore&
		operator =(VulkanSemaphore&& a_other) noexcept;

		virtual
		~VulkanSemaphore()
		{
			VulkanSemaphore::Destroy();
		};

		void
		Destroy() override;

		bool
		IsValid() const override;

		const vk::raii::Semaphore&
		GetVkSemaphore() const
		{
			return m_semaphore;
		}

		VulkanSemaphoreId
		GetId() const
		{
			return *m_semaphore;
		}

		operator VulkanSemaphoreId() const
		{
			return GetId();
		}

	protected:
		SemaphoreId
		GetIdImpl() const override
		{
			return GetId();
		}

	private:
		const VulkanDevice* m_device = nullptr;
		vk::raii::Semaphore m_semaphore = nullptr;
	};

	using VulkanSemaphoreHandle = PImpl<VulkanSemaphore>;
}
