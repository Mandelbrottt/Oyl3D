#include "VertexBuffer.h"

namespace Oyl::Rendering
{
	VertexBufferResource::VertexBufferResource() {}

	VertexBufferResource::VertexBufferResource(const byte* a_vertexData, size_t a_vertexLength)
	{
		SetVertexData(a_vertexData, a_vertexLength);
	}

	VertexBufferResource::VertexBufferResource(
		const byte* a_vertexData,
		size_t a_vertexLength,
		const byte* a_indexData,
		size_t a_indexLength
	)
		: VertexBufferResource(a_vertexData, a_vertexLength)
	{
		SetIndexData(a_indexData, a_indexLength);
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

		if (m_vertexData.empty())
			return false;

		m_hasIndexData = !m_indexData.empty();
		if (m_hasIndexData)
			m_vertexDataOffset = m_indexData.size();

		return DeviceResource::Load();
	}

	bool
	VertexBufferResource::Unload()
	{
		m_vertexData.clear();
		m_vertexData.shrink_to_fit();

		m_indexData.clear();
		m_indexData.shrink_to_fit();

		m_hasIndexData = false;
		m_vertexDataOffset = 0;

		return DeviceResource::Unload();
	}

	void
	VertexBufferResource::SetVertexData(const byte* a_vertexData, size_t a_vertexLength)
	{
		m_vertexData.clear();

		if (!a_vertexData || a_vertexLength == 0)
			return;

		m_vertexData.insert(m_vertexData.end(), &a_vertexData[0], &a_vertexData[a_vertexLength]);
		SetDirty();
	}

	void
	VertexBufferResource::SetIndexData(const byte* a_indexData, size_t a_indexLength)
	{
		m_indexData.clear();

		if (!a_indexData || a_indexLength == 0)
		{
			m_hasIndexData = false;
			m_vertexDataOffset = 0;
			return;
		}

		m_indexData.insert(m_indexData.end(), &a_indexData[0], &a_indexData[a_indexLength]);
		m_hasIndexData = true;
		m_vertexDataOffset = m_indexData.size();
		SetDirty();
	}

	bool
	VertexBufferResource::HasIndexData() const
	{
		return m_hasIndexData;
	}

	size_t
	VertexBufferResource::GetVertexDataOffset() const
	{
		return m_vertexDataOffset;
	}
}
