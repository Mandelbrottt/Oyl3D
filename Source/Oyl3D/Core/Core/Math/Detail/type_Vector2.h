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

		constexpr static TUnderlying size = VECTOR_SIZE;

		constexpr
		Vector_t()
			: x(0),
			  y(0) { }
		
		constexpr
		explicit
		Vector_t(TUnderlying a_value)
			: x(a_value),
			  y(a_value) { }

		constexpr
		Vector_t(TUnderlying a_x, TUnderlying a_y)
			: x(a_x),
			  y(a_y) { }
	
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
