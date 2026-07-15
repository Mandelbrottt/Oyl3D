#include "VertexBuffer.h"

namespace Oyl::Rendering
{
	VertexBufferResource::VertexBufferResource() {}

	VertexBufferResource::VertexBufferResource(const byte* a_data, size_t a_size)
	{
		m_data.insert(m_data.end(), &a_data[0], &a_data[a_size]);
	}

	VertexBufferResource::~VertexBufferResource()
	{
		VertexBufferResource::Unload();
	}

	bool
	VertexBufferResource::Load()
	{
		if (IsLoaded())
			return true;

		if (m_data.empty())
			return false;

		return DeviceResource::Load();
	}

	bool
	VertexBufferResource::Unload()
	{
		m_data.clear();
		m_data.shrink_to_fit();

		return DeviceResource::Unload();
	}
}
