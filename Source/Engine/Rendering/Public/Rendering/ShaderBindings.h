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
		uint location;
		TypeId type;
		uint size;
		uint offset;
	};

	struct ShaderBufferBinding
	{
		ShaderProfileFlagBits stage;

		String name;
		uint binding;
		uint stride;

		struct BufferMember
		{
			String name;
			TypeId type;

			uint stride;
			uint offset;
		};

		Array<BufferMember> members;
	};

}
