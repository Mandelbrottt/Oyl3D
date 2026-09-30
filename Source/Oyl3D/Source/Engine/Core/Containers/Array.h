#pragma once

#include <array>
#include <vector>

namespace Oyl
{
	template<typename T>
	class Array : public std::vector<T>
	{
	public:
		using ElementTy = T;
		using SizeTy = uint32_t;

		constexpr
		Array() = default;
		~Array() = default;

		constexpr
		Array(const Array& a_rhs) = default;
		//{
		//	*this = a_rhs;
		//}

		constexpr Array&
		operator =(const Array& a_rhs) = default;
		//{
		//	Array temp(a_rhs);
		//	std::swap(*this, temp);
		//	return *this;
		//}

		constexpr
		Array(Array&& a_rhs) noexcept = default;
		//{
		//	*this = std::move(a_rhs);
		//}

		constexpr Array&
		operator =(Array&& a_rhs) noexcept = default;
		//{
		//	if (this != &a_rhs)
		//	{
		//		std::swap(*this, a_rhs);
		//	}
		//	return *this;
		//}

		constexpr
		Array(const std::vector<T>& a_rhs)
		{
			*this = a_rhs;
		}

		constexpr Array&
		operator =(const std::vector<T>& a_rhs)
		{
			Array temp(a_rhs);
			std::swap(*this, temp);
			return *this;
		}

		constexpr
		Array(std::vector<T>&& a_rhs) noexcept
		{
			*this = std::move(a_rhs);
		}

		constexpr Array&
		operator =(std::vector<T>&& a_rhs) noexcept
		{
			if (this != &a_rhs)
			{
				std::swap(*this, a_rhs);
			}
			return *this;
		}

		constexpr
		void
		Add(const ElementTy& a_value)
		{
			std::vector<T>::push_back(a_value);
		}

		constexpr
		void
		Add(ElementTy&& a_value)
		{
			std::vector<T>::push_back(std::move(a_value));
		}

		constexpr SizeTy
		Size() const
		{
			return static_cast<SizeTy>(std::vector<T>::size());
		}

		constexpr SizeTy
		Length() const
		{
			return Size();
		}

		constexpr void
		Reserve(SizeTy a_size)
		{
			std::vector<T>::reserve(a_size);
		}
	};
}
