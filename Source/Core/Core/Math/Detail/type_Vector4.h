#pragma once

#include "type_Vector.h"

#pragma warning( push )
#pragma warning( disable : 26495 ) // Possibly uninitialized member

#define VECTOR_SIZE 4

namespace Oyl
{
	template<typename TUnderlying>
	struct Vector_t<VECTOR_SIZE, TUnderlying>
	{
		using value_type = TUnderlying;
		using type = Vector_t;
		
		constexpr static TUnderlying size = VECTOR_SIZE;

		constexpr
		explicit
		Vector_t(TUnderlying a_value = 0)
			: x(a_value),
			  y(a_value),
			  z(a_value),
			  w(a_value) { }

		constexpr
		Vector_t(TUnderlying a_x, TUnderlying a_y)
			: x(a_x),
			  y(a_y),
			  z(0),
			  w(0) { }

		constexpr
		Vector_t(TUnderlying a_x, TUnderlying a_y, TUnderlying a_z)
			: x(a_x),
			  y(a_y),
			  z(a_z),
			  w(0) { }

		constexpr
		Vector_t(TUnderlying a_x, TUnderlying a_y, TUnderlying a_z, TUnderlying a_w)
			: x(a_x),
			  y(a_y),
			  z(a_z),
			  w(a_w) { }
		
		constexpr
		Vector_t(Vector_t<3, TUnderlying> a_vector, TUnderlying a_w)
			: x(a_vector.data[0]),
			  y(a_vector.data[1]),
			  z(a_vector.data[2]),
			  w(a_w) { }
		
		_VECTOR_GENERATE_CONSTRUCTORS();

	#pragma warning( push )
	#pragma warning( disable : 4615 ) // Unknown user type
		union
		{
			struct
			{
				TUnderlying x;
				TUnderlying y;
				TUnderlying z;
				TUnderlying w;
			};

			struct
			{
				TUnderlying r;
				TUnderlying g;
				TUnderlying b;
				TUnderlying a;
			};

			TUnderlying data[VECTOR_SIZE];
		};
	#pragma warning( pop )
		
		_VECTOR_GENERATE_MEMBER_FUNCTIONS();
	};
}

#pragma warning( pop )

#undef VECTOR_SIZE
