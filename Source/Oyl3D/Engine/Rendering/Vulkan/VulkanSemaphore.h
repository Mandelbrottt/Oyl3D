#pragma once

#include <vulkan/vulkan.hpp>

#include "Rendering/Semaphore.h"

namespace Oyl::Rendering::Vulkan
{
	using SemaphoreHandle = ImplicitConversionWrapper<SemaphoreHandle, vk::Semaphore>;

	class Device;

	class OYL_RENDERING_API Semaphore : public ISemaphore
	{
	public:
		Semaphore();

		explicit
		Semaphore(const Device& a_device);

		Semaphore(Semaphore&& a_other) noexcept;
		Semaphore&
		operator =(Semaphore&& a_other) noexcept;

		virtual
		~Semaphore();

		void
		Destroy() override;

		bool
		IsValid() const override;

		const SemaphoreHandle&
		GetHandle() const override;

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}
