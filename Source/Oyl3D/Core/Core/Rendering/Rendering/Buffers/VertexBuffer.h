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

		VertexBufferResource(const byte* a_data, size_t a_size);

	public:
		virtual
		~VertexBufferResource();

		bool
		Load() override;

		bool
		Unload() override;

	protected:
		std::vector<byte> m_data;
	};

	using VertexBuffer = ResourceHandle<VertexBufferResource>;
}
