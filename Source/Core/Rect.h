#pragma once

#include "Core/Math/Vector.h"

namespace Oyl
{
	struct Rect2D
	{
		Rect2D(int32 a_x, int32 a_y, uint32 a_width, uint32 a_height)
			: x(a_x),
			  y(a_y),
			  width(a_width),
			  height(a_height) {}

		Rect2D(Vector2i a_position, Vector2u a_size)
			: position(a_position),
			  size(a_size) {}

		union
		{
			struct
			{
				int32 x;
				int32 y;

				uint32 width;
				uint32 height;
			};

			struct
			{
				Vector2i position;
				Vector2u size;
			};
		};
	};
}