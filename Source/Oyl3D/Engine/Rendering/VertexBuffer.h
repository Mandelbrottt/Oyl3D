#pragma once

#include <Core/Resources/Resource.h>
#include <Core/Resources/ResourceHandle.h>

namespace Oyl::Rendering
{
	struct VertexBufferOptions
	{
		const byte* vertexData;
		size_t vertexLength;

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
