#pragma once

#include "type_Vector.h"
#include "type_Vector4.h"

#define VECTOR_SIZE 2

namespace Oyl
{
	template<typename TUnderlying>
	struct Vector_t<VECTOR_SIZE, TUnderlying>
	{
		using value_type = TUnderlying;
		using type = Vector_t;

		static constexpr TUnderlying size = VECTOR_SIZE;

		static const Vector_t Zero;
		static const Vector_t One;
		static const Vector_t Up;
		static const Vector_t Down;
		static const Vector_t Right;
		static const Vector_t Left;

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

		_VECTOR_GENERATE_CONSTRUCTORS()

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

		_VECTOR_GENERATE_MEMBER_FUNCTIONS()
	};

	template<typename TUnderlying>
	constexpr Vector_t<VECTOR_SIZE, TUnderlying> Vector_t<VECTOR_SIZE, TUnderlying>::Zero = Vector_t(0);

	template<typename TUnderlying>
	constexpr Vector_t<VECTOR_SIZE, TUnderlying> Vector_t<VECTOR_SIZE, TUnderlying>::One = Vector_t(1);

	template<typename TUnderlying>
	constexpr Vector_t<VECTOR_SIZE, TUnderlying> Vector_t<VECTOR_SIZE, TUnderlying>::Up = Vector_t(0, 1);

	template<typename TUnderlying>
	constexpr Vector_t<VECTOR_SIZE, TUnderlying> Vector_t<VECTOR_SIZE, TUnderlying>::Down = Vector_t(0, -1);

	template<typename TUnderlying>
	constexpr Vector_t<VECTOR_SIZE, TUnderlying> Vector_t<VECTOR_SIZE, TUnderlying>::Right = Vector_t(1, 0);

	template<typename TUnderlying>
	constexpr Vector_t<VECTOR_SIZE, TUnderlying> Vector_t<VECTOR_SIZE, TUnderlying>::Left = Vector_t(-1, 0);
}

#undef VECTOR_SIZE
