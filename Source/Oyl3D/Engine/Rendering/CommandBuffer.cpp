#include "CommandBuffer.h"

#include "CommandPool.h"

namespace Oyl::Rendering
{
	CommandBuffer::CommandBuffer(const CommandPool* a_commandPool) noexcept
		: m_commandPool(a_commandPool) {}

	CommandBuffer::~CommandBuffer() noexcept
	{
		CommandBuffer::Destroy();
	}

	void
	CommandBuffer::Destroy() noexcept
	{
		m_commandPool = nullptr;
	}

	bool
	CommandBuffer::IsValid() const noexcept
	{
		return m_commandPool && m_commandPool->IsValid();
	}
}
