#pragma once

#include <Core/Resources/ResourceHandle.h>

#include "Rendering/DeviceResource.h"

namespace Oyl::Rendering
{
	struct VertexBufferOptions
	{
		const byte* vertexData;
		size_t vertexLength;
		uint32 vertexStride;

		const byte* indexData;
		size_t indexLength;
	};

	class OYL_RENDERING_API VertexBufferResource : public DeviceResource<VertexBufferResource>
	{
	protected:
		VertexBufferResource();

		explicit
		VertexBufferResource(const VertexBufferOptions& a_options);

	public:
		virtual
		~VertexBufferResource();

		bool
		Load() override;

		bool
		Unload() override;

		uint32
		GetVertexCount() const
		{
			return m_vertexCount;
		}

		uint32
		GetVertexSpan() const
		{
			return m_vertexStride;
		}

		void
		SetVertexData(const byte* a_vertexData, size_t a_vertexLength, uint32 a_vertexStride);

		uint32
		GetIndexCount() const
		{
			return m_indexCount;
		}

		bool
		HasIndexData() const;

		void
		SetIndexData(const byte* a_indexData, size_t a_indexLength);

		size_t
		GetVertexDataOffset() const;

	protected:
		uint32 m_vertexCount = 0;
		uint32 m_vertexStride = 0;
		std::vector<byte> m_vertexData;

		uint32 m_indexCount = 0;
		std::vector<byte> m_indexData;

		bool m_hasIndexData = false;
		size_t m_vertexDataOffset = 0;
	};

	using VertexBuffer = ResourceHandle<VertexBufferResource>;
}
