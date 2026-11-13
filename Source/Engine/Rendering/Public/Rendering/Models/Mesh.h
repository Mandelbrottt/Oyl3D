#pragma once

#include "Core/Math/Vector.h"

#include "Rendering/Hardware/Image.h"

namespace Oyl
{
	class Mesh
	{
	public:
		Mesh() = default;

		Mesh(
			const Array<Vector3>& a_vertexPositions,
			const Array<Vector2>& a_vertexUvs,
			const Array<Vector3>& a_vertexNormals,
			const Array<uint32>& a_vertexIndices,
			const RdoImageHandle* a_texture = nullptr
		)
			: m_vertexPositions(a_vertexPositions),
			  m_vertexUvs(a_vertexUvs),
			  m_vertexNormals(a_vertexNormals),
			  m_vertexIndices(a_vertexIndices)
		{
			if (a_texture)
				m_texture = *a_texture;
		}

		const Array<Vector3>&
		GetVertexPositions() const { return m_vertexPositions; }

		const Array<Vector2>&
		GetVertexUvs() const { return m_vertexUvs; }

		const Array<Vector3>&
		GetVertexNormals() const { return m_vertexNormals; }

		const Array<uint32>&
		GetVertexIndices() const { return m_vertexIndices; }

	private:
		Array<Vector3> m_vertexPositions;
		Array<Vector2> m_vertexUvs;
		Array<Vector3> m_vertexNormals;
		Array<uint32> m_vertexIndices;

		RdoImageHandle m_texture = nullptr;
	};
}
