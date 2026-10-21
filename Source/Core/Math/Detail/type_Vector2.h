#pragma once

#include "type_Vector.h"

#define VECTOR_SIZE 2

namespace Oyl
{
	template<typename TUnderlying>
	struct Vector_t<VECTOR_SIZE, TUnderlying>
	{
		using value_type = TUnderlying;
		using type = Vector_t;

		static constexpr TUnderlying size = VECTOR_SIZE;

		static
		constexpr Vector_t
		Zero() { return Vector_t(0); }

		static
		constexpr Vector_t
		One() { return Vector_t(1); }

		constexpr
		Vector_t()
			: x(0),
			  y(0) {}

		explicit
		constexpr
		Vector_t(TUnderlying a_value)
			: x(a_value),
			  y(a_value) {}

		constexpr
		Vector_t(TUnderlying a_x, TUnderlying a_y)
			: x(a_x),
			  y(a_y) {}

		_VECTOR_GENERATE_CONSTRUCTORS();

	#pragma warning( push )
	#pragma warning( disable : 4201 ) // nameless struct/union
		union
		{
			struct
			{
				TUnderlying x;
				TUnderlying y;
			};

			struct
			{
				TUnderlying u;
				TUnderlying v;
			};

			TUnderlying data[VECTOR_SIZE];
		};
	#pragma warning( pop )

		_VECTOR_GENERATE_MEMBER_FUNCTIONS();
	};
}

#undef VECTOR_SIZE
