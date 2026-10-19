#pragma once

#include <array>
#include <vector>

#include "Core/Types/Primitives.h"

namespace Oyl
{
	template<typename T>
	class Array : public std::vector<T>
	{
	public:
		using ElementTy = T;
		using SizeTy = uint32;

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
		bool
		Empty() const
		{
			return Length() == 0;
		}

		constexpr
		bool
		Contains(const ElementTy& a_value) const
		{
			return std::ranges::find(*this, a_value) != this->end();
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

		void
		Clear()
		{
			std::vector<T>::clear();
		}
	};

	template<typename TElement>
	class ArrayProxy : public Array<TElement>
	{
	public:
		using ArrayTy = Array<TElement>;
		using ElementTy = typename ArrayTy::ElementTy;
		using SizeTy = typename ArrayTy::SizeTy;

		constexpr
		ArrayProxy()
			: ArrayTy() {}

		constexpr
		ArrayProxy(const ElementTy& a_element)
			: ArrayTy()
		{
			this->Add(a_element);
		}

		constexpr
		ArrayProxy(ElementTy&& a_element)
			: ArrayTy()
		{
			this->Add(std::move(a_element));
		}

		ArrayProxy(const ArrayTy& a_array)
			: ArrayTy(a_array) {}

		ArrayProxy(ArrayTy&& a_vector)
			: ArrayTy(std::move(a_vector)) {}

		ArrayProxy(const std::vector<ElementTy>& a_vector)
			: ArrayTy(a_vector) {}

		ArrayProxy(std::vector<ElementTy>&& a_vector)
			: ArrayTy(std::move(a_vector)) {}
	};
}
