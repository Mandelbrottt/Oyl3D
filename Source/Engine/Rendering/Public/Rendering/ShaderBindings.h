#pragma once

#include <Core/Containers/Array.h>
#include <Core/Types/TypeId.h>

#include "Rendering/RenderingEnums.h"

namespace Oyl
{
	struct ShaderInputBinding
	{
		ShaderProfileFlagBits stage;

		String name;
		uint32 location;

		TypeId type;
		uint32 size;
		uint32 offset;
	};

	struct ShaderBufferBinding
	{
		ShaderProfileFlagBits stage;

		String name;
		uint32 binding;
		uint32 stride;

		struct BufferMember
		{
			String name;
			TypeId type;

			uint32 stride;
			uint32 offset;
		};

		Array<BufferMember> members;
	};

}
