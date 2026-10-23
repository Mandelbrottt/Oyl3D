#pragma once

#include <vulkan/vulkan_raii.hpp>

#include <Rendering/Semaphore.h>

namespace Oyl
{
	class VulkanDevice;

	using VulkanSemaphoreId = IConvertibleDeviceObjectId<SemaphoreId, vk::Semaphore>;

	class VULKANMODULE_EXPORT VulkanSemaphore : public ISemaphore
	{
	public:
		VulkanSemaphore(nullptr_t) {}

		VulkanSemaphore(const VulkanDevice& a_device);

		DEFAULT_MOVE(VulkanSemaphore);

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
		GetVkSemaphore() const { return m_semaphore; }

		VulkanSemaphoreId
		GetId() const { return *m_semaphore; }

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
}
