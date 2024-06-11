#pragma once

#include "type_Vector.h"

#pragma warning( push )
#pragma warning( disable : 26495 ) // Possibly uninitialized member

#define VECTOR_SIZE 3

namespace Oyl
{
	namespace Vector
	{
		template<typename TUnderlying>
		constexpr
		Vector_t<3, TUnderlying> Cross(Vector_t<3, TUnderlying>, Vector_t<3, TUnderlying>);
	}

	template<typename TUnderlying>
	struct Vector_t<VECTOR_SIZE, TUnderlying>
	{
		using value_type = TUnderlying;
		using type = Vector_t;
		
		constexpr static TUnderlying size = VECTOR_SIZE;

		constexpr
		Vector_t()
			: x(0),
			  y(0),
			  z(0) { }
		
		constexpr
		explicit
		Vector_t(TUnderlying a_value)
			: x(a_value),
			  y(a_value),
			  z(a_value) { }

		constexpr
		Vector_t(TUnderlying a_x, TUnderlying a_y)
			: x(a_x),
			  y(a_y),
			  z(0) { }
		
		constexpr
		Vector_t(TUnderlying a_x, TUnderlying a_y, TUnderlying a_z)
			: x(a_x),
			  y(a_y),
			  z(a_z) { }
		
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
			};

			struct
			{
				TUnderlying r;
				TUnderlying g;
				TUnderlying b;
			};

			TUnderlying data[VECTOR_SIZE];
		};
	#pragma warning( pop )
		
		_VECTOR_GENERATE_MEMBER_FUNCTIONS();

		constexpr
		Vector_t<3, TUnderlying>
		Cross(Vector_t<3, TUnderlying> a_other)
		{
			return Vector::Cross(*this, a_other);
		}

		constexpr static Vector_t<3, TUnderlying> Zero()     { return Vector_t<3, TUnderlying>(0, 0, 0); }
		
		constexpr static Vector_t<3, TUnderlying> One()      { return Vector_t<3, TUnderlying>(1, 1, 1); }
		
		constexpr static Vector_t<3, TUnderlying> Right()    { return Vector_t<3, TUnderlying>(1, 0, 0); }

		constexpr static Vector_t<3, TUnderlying> Left()     { return Vector_t<3, TUnderlying>(-1, 0, 0); }
														   
		constexpr static Vector_t<3, TUnderlying> Up()       { return Vector_t<3, TUnderlying>(0, 1, 0); }

		constexpr static Vector_t<3, TUnderlying> Down()     { return Vector_t<3, TUnderlying>(0, -1, 0); }

		constexpr static Vector_t<3, TUnderlying> Forward()  { return Vector_t<3, TUnderlying>(0, 0, 1); }

		constexpr static Vector_t<3, TUnderlying> Backward() { return Vector_t<3, TUnderlying>(0, 0, -1); }
	};

	namespace Vector
	{
		template<typename TUnderlying>
		constexpr
		Vector_t<3, TUnderlying>
		Cross(Vector_t<3, TUnderlying> a_lhs, Vector_t<3, TUnderlying> a_rhs)
		{
			TUnderlying x_component = a_lhs.y * a_rhs.z - a_lhs.z * a_rhs.y;
			TUnderlying y_component = a_lhs.z * a_rhs.x - a_lhs.x * a_rhs.z;
			TUnderlying z_component = a_lhs.x * a_rhs.y - a_lhs.y * a_rhs.x;

			return Vector_t<3, TUnderlying>(x_component, y_component, z_component);
		}
	}
}

#pragma warning( pop )

#undef VECTOR_SIZE
