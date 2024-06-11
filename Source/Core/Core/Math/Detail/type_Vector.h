#pragma once

#include <cmath>

#include "Core/Common.h"
#include "Core/Math/Constants.h"

namespace Oyl
{
	/**
	 * \brief Representation of a mathematical vector
	 * \tparam Size The number of elements in the vector
	 * \tparam TUnderlying The underlying data type
	 * \remarks The name uses _t type notation because for the API we want, there is no way to
	 *          have a template and non-template type share an identifier.
	 *          ie.
	 *          \code
	 *          struct Vector
	 *          {
	 *		        ...
	 *		    }
	 *		    template<...>
	 *		    struct Vector
	 *		    {
	 *		        ...
	 *		    } // Compile error, redefinition 
	 *          \endcode
	 *      <br>All vector specializations should implement the
	 *          _VECTOR_GENERATE_CONSTRUCTORS() and _VECTOR_GENERATE_MEMBER_FUNCTIONS() macros.
	 */
	template<int Size, typename TUnderlying>
	struct Vector_t
	{
		using value_type = TUnderlying;
		using type = Vector_t;

		constexpr static int size = Size;

		TUnderlying data[Size];
	};

	namespace Vector
	{
		template<int Size, typename TUnderlying>
		constexpr
		TUnderlying
		Dot(Vector_t<Size, TUnderlying> const& a_lhs, Vector_t<Size, TUnderlying> const& a_rhs)
		{
			TUnderlying result = 0;
			for (int i = 0; i < Size; i++)
			{
				result += a_lhs.data[i] * a_rhs.data[i];
			}
			return result;
		}

		template<int Size, typename TUnderlying>
		constexpr
		float
		MagnitudeSquared(Vector_t<Size, TUnderlying> const& a_value)
		{
			TUnderlying result = 0;
			for (int i = 0; i < Size; i++)
			{
				result += a_value.data[i] * a_value.data[i];
			}
			return static_cast<float>(result);
		}

		template<int Size, typename TUnderlying>
		constexpr
		float
		Magnitude(Vector_t<Size, TUnderlying> const& a_value)
		{
			float result = MagnitudeSquared(a_value);
			return std::sqrt(result);
		}

		template<int Size, typename TUnderlying>
		constexpr
		Vector_t<Size, TUnderlying>
		Normalize(Vector_t<Size, TUnderlying> const& a_value)
		{
			float magnitude = Magnitude(a_value);
			if (magnitude == 0)
			{
				return a_value;
			}
			return a_value / magnitude;
		}
		
		template<int Size, typename TUnderlying>
		constexpr
		float
		Angle(Vector_t<Size, TUnderlying> const& a_lhs, Vector_t<Size, TUnderlying> const& a_rhs)
		{
			float cos_theta = Vector::Dot(Normalize(a_lhs), Normalize(a_rhs));
			
			return std::acos(cos_theta) * Math::RAD_TO_DEG;
		}
	}

	template<int Size, typename TUnderlying>
	constexpr
	Vector_t<Size, TUnderlying>
	operator +(Vector_t<Size, TUnderlying> a_lhs, Vector_t<Size, TUnderlying> a_rhs)
	{
		Vector_t<Size, TUnderlying> result;
		for (int i = 0; i < Size; i++)
		{
			result.data[i] = a_lhs.data[i] + a_rhs.data[i];
		}
		return result;
	}

	template<int Size, typename TUnderlying>
	constexpr
	Vector_t<Size, TUnderlying>
	operator -(Vector_t<Size, TUnderlying> a_lhs, Vector_t<Size, TUnderlying> a_rhs)
	{
		Vector_t<Size, TUnderlying> result;
		for (int i = 0; i < Size; i++)
		{
			result.data[i] = a_lhs.data[i] - a_rhs.data[i];
		}
		return result;
	}

	template<int Size, typename TUnderlying>
	constexpr
	Vector_t<Size, TUnderlying>
	operator *(Vector_t<Size, TUnderlying> a_lhs, Vector_t<Size, TUnderlying> a_rhs) noexcept
	{
		Vector_t<Size, TUnderlying> result;
		for (int i = 0; i < Size; i++)
		{
			result.data[i] = a_lhs.data[i] * a_rhs.data[i];
		}
		return result;
	}

	template<int Size, typename TUnderlying>
	constexpr
	Vector_t<Size, TUnderlying>
	operator /(Vector_t<Size, TUnderlying> a_lhs, Vector_t<Size, TUnderlying> a_rhs) noexcept
	{
		Vector_t<Size, TUnderlying> result;
		for (int i = 0; i < Size; i++)
		{
			result.data[i] = a_lhs.data[i] / a_rhs.data[i];
		}
		return result;
	}

	template<int Size, typename TUnderlying>
	constexpr
	Vector_t<Size, TUnderlying>
	operator *(Vector_t<Size, TUnderlying> a_lhs, TUnderlying a_rhs) noexcept
	{
		Vector_t<Size, TUnderlying> result;
		for (int i = 0; i < Size; i++)
		{
			result.data[i] = a_lhs.data[i] * a_rhs;
		}
		return result;
	}

	template<int Size, typename TUnderlying>
	constexpr
	Vector_t<Size, TUnderlying>
	operator /(Vector_t<Size, TUnderlying> a_lhs, TUnderlying a_rhs) noexcept
	{
		Vector_t<Size, TUnderlying> result;
		for (int i = 0; i < Size; i++)
		{
			result.data[i] = a_lhs.data[i] / a_rhs;
		}
		return result;
	}

	template<int Size, typename TUnderlying>
	constexpr
	Vector_t<Size, TUnderlying>
	operator -(Vector_t<Size, TUnderlying> const& a_value) noexcept
	{
		Vector_t<Size, TUnderlying> result;
		for (int i = 0; i < Size; i++)
		{
			result.data[i] = -a_value.data[i];
		}
		return result;
	}

	template<int Size, typename TUnderlying>
	constexpr
	bool
	operator ==(Vector_t<Size, TUnderlying> const& a_lhs, Vector_t<Size, TUnderlying> const& a_rhs) noexcept
	{
		bool result = true;
		for (int i = 0; i < Size; i++)
		{
			result &= a_lhs.data[i] == a_rhs.data[i];
		}
		return result;
	}

	template<int Size, typename TUnderlying>
	constexpr
	bool
	operator !=(Vector_t<Size, TUnderlying> const& a_lhs, Vector_t<Size, TUnderlying> const& a_rhs) noexcept
	{
		return !(a_lhs == a_rhs);
	}
	
	#define _VECTOR_DEFINE_BINARY_OPERATOR_MEMBER_SAME_TYPE(_operator_) \
		constexpr\
		Vector_t& \
		operator _operator_##=(Vector_t a_other) noexcept \
		{ \
			*this = *this _operator_ a_other;\
			return *this; \
		} \
		_OYL_REQUIRE_SEMICOLON

	#define _VECTOR_DEFINE_BINARY_OPERATOR_MEMBER_UNDERLYING_TYPE(_operator_) \
		constexpr\
		Vector_t& \
		operator _operator_##=(TUnderlying a_other) noexcept \
		{ \
			*this = *this _operator_ a_other;\
			return *this; \
		} \
		_OYL_REQUIRE_SEMICOLON

	#define _VECTOR_GENERATE_CONSTRUCTORS() \
		template<int Size>\
		constexpr \
		explicit \
		Vector_t(Vector_t<Size, TUnderlying> a_other) \
		{ \
			constexpr int min_index = VECTOR_SIZE < Size ? VECTOR_SIZE : Size; \
			for (int i = 0; i < min_index; i++) \
			{ \
				data[i] = a_other.data[i]; \
			} \
		} \
		_OYL_REQUIRE_SEMICOLON
	
	#define _VECTOR_GENERATE_MEMBER_FUNCTIONS() \
		constexpr \
		float \
		MagnitudeSquared() noexcept \
		{ \
			return ::Oyl::Vector::MagnitudeSquared(*this); \
		} \
		\
		constexpr \
		float \
		Magnitude() noexcept\
		{\
			return ::Oyl::Vector::Magnitude(*this); \
		}\
		\
		constexpr \
		type& \
		Normalize() noexcept \
		{\
			return (*this = ::Oyl::Vector::Normalize(*this)); \
		}\
		\
		constexpr \
		TUnderlying \
		Dot(type const& a_other) noexcept \
		{ \
			return ::Oyl::Vector::Dot(*this, a_other); \
		} \
		\
		_VECTOR_DEFINE_BINARY_OPERATOR_MEMBER_SAME_TYPE(+); \
		_VECTOR_DEFINE_BINARY_OPERATOR_MEMBER_SAME_TYPE(-); \
		_VECTOR_DEFINE_BINARY_OPERATOR_MEMBER_SAME_TYPE(*); \
		_VECTOR_DEFINE_BINARY_OPERATOR_MEMBER_SAME_TYPE(/); \
		_VECTOR_DEFINE_BINARY_OPERATOR_MEMBER_UNDERLYING_TYPE(*); \
		_VECTOR_DEFINE_BINARY_OPERATOR_MEMBER_UNDERLYING_TYPE(/); \
		\
		constexpr \
		TUnderlying& \
		operator [](int a_index)\
		{ \
			if (a_index >= size) \
			{ \
				throw "Index out of range!";\
			} \
			return data[a_index];\
		} \
		\
		_OYL_REQUIRE_SEMICOLON
}
