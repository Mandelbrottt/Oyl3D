#pragma once

#include <Core/Math/Vector.h>
#include <Core/Resources/Resource.h>
#include <Core/Resources/ResourceHandle.h>

#include "Rendering/Common.h"

namespace Oyl::Rendering
{
	class OYL_RENDERING_API VertexBufferResource : public DeviceResource<VertexBufferResource>
	{
	protected:
		VertexBufferResource();

		VertexBufferResource(const byte* a_vertexData, size_t a_vertexLength);

		VertexBufferResource(
			const byte* a_vertexData,
			size_t a_vertexLength,
			const byte* a_indexData,
			size_t a_indexLength
		);

	public:
		virtual
		~VertexBufferResource();

		bool
		Load() override;

		bool
		Unload() override;

		void
		SetVertexData(const byte* a_vertexData, size_t a_vertexLength);

		void
		SetIndexData(const byte* a_indexData, size_t a_indexLength);

		bool
		HasIndexData() const;

		size_t
		GetVertexDataOffset() const;

	protected:
		std::vector<byte> m_vertexData;
		std::vector<byte> m_indexData;

		bool m_hasIndexData = false;
		size_t m_vertexDataOffset = 0;
	};

	using VertexBuffer = ResourceHandle<VertexBufferResource>;
}
