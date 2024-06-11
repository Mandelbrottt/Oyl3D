#pragma once

#pragma once

#include "type_Matrix.h"
#include "type_Matrix3.h"
#include "type_Vector4.h"

#include "Core/Math/Vector3.h"
#include "Core/Math/Vector4.h"

#pragma warning( push )
#pragma warning( disable : 26495 ) // Possibly uninitialized member

#define MATRIX_SIZE 4

namespace Oyl
{
	template<typename TUnderlying>
	struct Matrix_t<MATRIX_SIZE, MATRIX_SIZE, TUnderlying>
	{
		using value_type = TUnderlying;
		using type = Matrix_t<MATRIX_SIZE, MATRIX_SIZE, TUnderlying>;

		constexpr static int size_x = MATRIX_SIZE;
		constexpr static int size_y = MATRIX_SIZE;

		constexpr
		Matrix_t()
			: Matrix_t(0) {}

		constexpr
		explicit
		Matrix_t(TUnderlying a_value)
			: data { a_value } { }

		constexpr
		Matrix_t(
			Vector_t<MATRIX_SIZE, TUnderlying> a_x,
			Vector_t<MATRIX_SIZE, TUnderlying> a_y,
			Vector_t<MATRIX_SIZE, TUnderlying> a_z,
			Vector_t<MATRIX_SIZE, TUnderlying> a_w
		) : cols { a_x, a_y, a_z, a_w } {}
		
		constexpr
		Matrix_t(
			Matrix_t<3, 3, TUnderlying> a_matrix
		) : cols {
			Vector_t<4, TUnderlying>(a_matrix.cols[0], 0),
			Vector_t<4, TUnderlying>(a_matrix.cols[1], 0),
			Vector_t<4, TUnderlying>(a_matrix.cols[2], 0),
			Vector_t<4, TUnderlying>(Vector_t<3, TUnderlying>(0), 1),
		} {}
		
		explicit
		constexpr
		operator
		Matrix_t<3, 3, TUnderlying>()
		{
			return Matrix_t<3, 3, TUnderlying> {
				Vector_t<3, TUnderlying>(cols[0]),
				Vector_t<3, TUnderlying>(cols[1]),
				Vector_t<3, TUnderlying>(cols[2])
			};
		}
		
		_MATRIX_GENERATE_CONSTRUCTORS();
		
	#pragma warning( push )
	#pragma warning( push )
	#pragma warning( disable : 4615 ) // Unknown user type
		union
		{
			struct
			{
				TUnderlying m00, m01, m02, m03;
				TUnderlying m10, m11, m12, m13;
				TUnderlying m20, m21, m22, m23;
				TUnderlying m30, m31, m32, m33;
			};

			Vector_t<size_x, TUnderlying> cols[size_y];

			TUnderlying data[size_x * size_y];
		};
	#pragma warning( pop )

		_MATRIX_GENERATE_MEMBER_FUNCTIONS();
	};
}

#pragma warning( pop )

#undef MATRIX_SIZE
