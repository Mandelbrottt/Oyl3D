#pragma once

#include <cstring>
#include <cstdint>
#include <cmath>

#include "Core/Common.h"
#include "type_Vector.h"

namespace Oyl
{
	/**
	 * \brief Row-Major arbitrary-dimensional representation of a matrix
	 * \tparam SizeX The number of columns in the matrix
	 * \tparam SizeY The number of rows in the matrix
	 * \tparam TUnderlying The underlying data type
	 * \remark The name uses _t type notation because for the API we want, there is no way to
	 *         have a template and non-template type share an identifier.
	 *         ie.
	 *         \code
	 *         struct Matrix
	 *         {
	 *		       ...
	 *		   }
	 *		   template<...>
	 *		   struct Matrix
	 *		   {
	 *		       ...
	 *		   } // Compile error, redefinition
	 *		   \endcode
	 *	   <br>All matrix specializations should implement the _MATRIX_GENERATE_MEMBER_FUNCTIONS() macro.
	 */
	template<int SizeX, int SizeY, typename TUnderlying>
	struct Matrix_t
	{
		using value_type = TUnderlying;
		using type = Matrix_t;

		constexpr static int size_x = SizeX;
		constexpr static int size_y = SizeY;

		TUnderlying data[SizeX * SizeY];
	};

	namespace Matrix
	{
		template<typename TMatrix>
		static
		constexpr
		TMatrix
		Identity(typename TMatrix::value_type a_diagonal = 1) noexcept
		{
			constexpr int size_x = TMatrix::size_x;
			constexpr int size_y = TMatrix::size_y;

			constexpr int min_size = size_x < size_y ? size_x : size_y;

			TMatrix result { 0 };
			for (int i = 0; i < min_size; i++)
			{
				result.data[i * size_x + i] = a_diagonal;
			}
			return result;
		}

		template<int SizeX, int SizeY = SizeX, typename TUnderlying = float>
		static
		constexpr
		Matrix_t<SizeX, SizeY, TUnderlying>
		Identity(TUnderlying a_diagonal = 1) noexcept
		{
			using TMatrix = Matrix_t<SizeX, SizeY, TUnderlying>;
			return Identity<TMatrix>(a_diagonal);
		}

		template<int SizeX, int SizeY, typename TUnderlying>
		constexpr
		Matrix_t<SizeY, SizeX, TUnderlying>
		Transpose(const Matrix_t<SizeX, SizeY, TUnderlying>& a_value) noexcept
		{
			//constexpr int min_size = SizeX < SizeY ? SizeX : SizeY;

			Matrix_t<SizeY, SizeX, TUnderlying> result;

			for (int y = 0; y < SizeY; y++)
			{	
				for (int x = 0; x < SizeX; x++)
				{
					//result[x][y] = a_value[y][x];
					result.data[x * SizeY + y] = a_value.data[y * SizeX + x];
				}
			}

			return result;
		}

		template<int SizeX, int SizeY, typename TUnderlying>
		constexpr
		Matrix_t<SizeX, SizeY, TUnderlying>
		Inverse(const Matrix_t<SizeX, SizeY, TUnderlying>& a_value) noexcept
		{
			// TODO: Implement Inverse and Determinant
		}
	}
	
	// Matrix-Matrix Addition
	template<int SizeX, int SizeY, typename TUnderlying>
	constexpr
	Matrix_t<SizeX, SizeY, TUnderlying>
	operator +(const Matrix_t<SizeX, SizeY, TUnderlying>& a_lhs, const Matrix_t<SizeX, SizeY, TUnderlying>& a_rhs) noexcept
	{
		Matrix_t<SizeX, SizeY, TUnderlying> result;
		for (int i = 0; i < SizeX * SizeY; i++)
		{
			result.data[i] = a_lhs.data[i] + a_rhs.data[i];
		}
		return result;
	}

	// Matrix-Matrix Subtraction
	template<int SizeX, int SizeY, typename TUnderlying>
	constexpr
	Matrix_t<SizeX, SizeY, TUnderlying>
	operator -(const Matrix_t<SizeX, SizeY, TUnderlying>& a_lhs, const Matrix_t<SizeX, SizeY, TUnderlying>& a_rhs) noexcept
	{
		Matrix_t<SizeX, SizeY, TUnderlying> result;
		for (int i = 0; i < SizeX * SizeY; i++)
		{
			result.data[i] = a_lhs.data[i] - a_rhs.data[i];
		}
		return result;
	}

	// Matrix-Scalar Multiplication
	template<int SizeX, int SizeY, typename TUnderlying>
	constexpr
	Matrix_t<SizeX, SizeY, TUnderlying>
	operator *(Matrix_t<SizeX, SizeY, TUnderlying> a_lhs, TUnderlying a_rhs) noexcept
	{
		Matrix_t<SizeX, SizeY, TUnderlying> result;
		for (int i = 0; i < SizeX * SizeY; i++)
		{
			result.data[i] = a_lhs.data[i] * a_rhs;
		}
		return result;
	}

	// Matrix-Matrix Multiplication
	template<int SizeShared, int SizeLhsY, int SizeRhsX, typename TUnderlying>
	constexpr
	Matrix_t<SizeShared, SizeLhsY, TUnderlying>
	operator *(Matrix_t<SizeShared, SizeLhsY, TUnderlying> a_lhs, Matrix_t<SizeRhsX, SizeShared, TUnderlying> a_rhs) noexcept
	{
		Matrix_t<SizeShared, SizeLhsY, TUnderlying> result { 0 };

		const auto sizeX      = SizeRhsX;
		const auto sizeY      = SizeLhsY;
		const auto sizeShared = SizeShared;

		// ReSharper disable CppBadChildStatementIndent
		for (int y = 0; y < SizeLhsY; y++)
			for (int x = 0; x < SizeRhsX; x++)
				for (int z = 0; z < SizeShared; z++)
				{
					auto        leftIndex = y * SizeShared + z;
					TUnderlying left      = a_lhs.data[leftIndex];

					auto        rightIndex = z * SizeRhsX + x;
					TUnderlying right      = a_rhs.data[rightIndex];

					TUnderlying product = left * right;
					result.data[y * SizeShared + x] += product;
				}
		// ReSharper restore CppBadChildStatementIndent

		return result;
	}

	// Matrix-Vector Multiplication Specialization
	template<int SizeShared, int SizeRhsX, typename TUnderlying>
	constexpr
	Vector_t<SizeShared, TUnderlying>
	operator *(Vector_t<SizeShared, TUnderlying> a_lhs, Matrix_t<SizeRhsX, SizeShared, TUnderlying> a_rhs) noexcept
	{
		constexpr int size_shared = SizeShared;
		constexpr int size_rhs_x  = SizeRhsX;

		// Don't use SizeShared directly, causes stack corruptions on some systems
		std::size_t sizeof_data = sizeof(TUnderlying) * size_shared;

		Matrix_t<SizeShared, 1, TUnderlying> lhsMatrix;
		std::memcpy(lhsMatrix.data, a_lhs.data, sizeof_data);

		Matrix_t<SizeShared, 1, TUnderlying> product = lhsMatrix * a_rhs;

		Vector_t<SizeShared, TUnderlying> result;
		std::memcpy(result.data, product.data, sizeof_data);

		return result;
	}
	
	#define _MATRIX_GENERATE_CONSTRUCTORS() \
		template<int SizeX, int SizeY> \
		constexpr \
		explicit \
		Matrix_t(Matrix_t<SizeX, SizeY, TUnderlying> const& a_other) noexcept \
		{ \
			constexpr int min_x = SizeX < size_x ? SizeX : size_x; \
			constexpr int min_y = SizeY < size_y ? SizeY : size_y; \
			\
			for (int y = 0; y < min_y; y++)  \
				for (int x = 0; x < min_x; x++) \
				{ \
					data[y * size_x + x] = a_other.data[y * SizeX + x]; \
				}  \
		} \
		\
		template<int SizeX, int SizeY> \
		constexpr \
		Matrix_t& \
		operator =(Matrix_t<SizeX, SizeY, TUnderlying> const& a_other) noexcept \
		{ \
			constexpr int min_x = SizeX < size_x ? SizeX : size_x; \
			constexpr int min_y = SizeY < size_y ? SizeY : size_y; \
			\
			for (int y = 0; y < min_y; y++)  \
				for (int x = 0; x < min_x; x++) \
				{ \
					data[y * size_x + x] = a_other.data[y * SizeX + x]; \
				} \
		} \
		_OYL_REQUIRE_SEMICOLON
		
	#define _MATRIX_GENERATE_MEMBER_FUNCTIONS() \
		constexpr \
		Matrix_t& \
		operator *=(Matrix_t a_other) noexcept \
		{ \
			*this = *this * a_other; \
			return *this; \
		} \
		\
		constexpr \
		Vector_t<size_x, TUnderlying>& \
		operator [](int a_index) \
		{ \
			if (a_index >= size_y) \
			{ \
				throw "Index out of range!";\
			} \
			return cols[a_index]; \
		} \
		\
		constexpr \
		static \
		type \
		Identity(TUnderlying a_diagonal = 1) noexcept  \
		{ \
			return ::Oyl::Matrix::Identity<type>(a_diagonal); \
		} \
		\
		constexpr \
		type& \
		Transpose() noexcept  \
		{ \
			*this = ::Oyl::Matrix::Transpose(*this); \
			return *this; \
		} \
		_OYL_REQUIRE_SEMICOLON // Require caller to end line with a semicolon
}
