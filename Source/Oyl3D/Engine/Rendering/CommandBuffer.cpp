#include "CommandBuffer.h"

#include "CommandPool.h"

namespace Oyl::Rendering
{
	CommandBuffer::CommandBuffer(const CommandPool* a_commandPool)
		: m_commandPool(a_commandPool) {}

	CommandBuffer::~CommandBuffer()
	{
		CommandBuffer::Destroy();
	}

	void
	CommandBuffer::Destroy()
	{
		m_commandPool = nullptr;
	}

	bool
	CommandBuffer::IsValid() const
	{
		return m_commandPool && m_commandPool->IsValid();
	}
}
