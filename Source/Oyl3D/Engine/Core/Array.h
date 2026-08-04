#pragma once

#include <array>
#include <vector>

#include "TypeTraits.h"

#include "Reflection/TypeId.h"

namespace Oyl
{
	class ArrayBase
	{
	protected:
		explicit
		ArrayBase(Reflection::TypeId a_elementType)
			: m_elementType(a_elementType) {}

	private:
		Reflection::TypeId m_elementType;
	};

	template<typename TElement>
	class Array : public ArrayBase
	{
	public:
		using ElementType = TElement;
		using SizeType = uint32;

		constexpr
		Array()
			: ArrayBase(typeidof(ElementType)) {}

		explicit
		constexpr
		Array(SizeType a_size)
			: ArrayBase(typeidof(ElementType)),
			  m_vector(a_size) {}

		constexpr
		Array(const Array& a_other)
			: Array()
		{
			*this = a_other;
		}

		constexpr Array&
		operator =(const Array& a_other)
		{
			if (this != &a_other)
			{
				m_vector = a_other.m_vector;
			}
			return *this;
		}

		constexpr
		Array(Array&& a_other) noexcept
			: Array()
		{
			*this = std::move(a_other);
		}

		constexpr Array&
		operator =(Array&& a_other) noexcept
		{
			if (this != &a_other)
			{
				std::swap(m_vector, a_other.m_vector);
			}
			return *this;
		}

		explicit
		Array(const std::vector<ElementType>& a_vector)
			: Array()
		{
			m_vector = a_vector;
		}

		explicit
		Array(std::vector<ElementType>&& a_vector)
			: Array()
		{
			m_vector = std::move(a_vector);
		}

		constexpr SizeType
		Size() const
		{
			return (SizeType) m_vector.size();
		}

		constexpr bool
		Empty() const
		{
			return Size() == 0;
		}

		constexpr SizeType
		Capacity() const
		{
			return (SizeType) m_vector.capacity();
		}

		ElementType*
		Data()
		{
			return m_vector.data();
		}

		const ElementType*
		Data() const
		{
			return m_vector.data();
		}

		ElementType&
		operator [](SizeType a_index)
		{
			return At(a_index);
		}

		const ElementType&
		operator [](SizeType a_index) const
		{
			return At(a_index);
		}

		ElementType&
		At(SizeType a_index)
		{
			return m_vector.at(a_index);
		}

		const ElementType&
		At(SizeType a_index) const
		{
			return const_cast<Array*>(this)->At(a_index);
		}

		ElementType&
		Front()
		{
			return m_vector.front();
		}

		const ElementType&
		Front() const
		{
			return const_cast<Array*>(this)->Front();
		}

		ElementType&
		Back()
		{
			return m_vector.back();
		}

		const ElementType&
		Back() const
		{
			return const_cast<Array*>(this)->Back();
		}

		constexpr void
		Reserve(SizeType a_capacity)
		{
			m_vector.reserve(a_capacity);
		}

		constexpr void
		Resize(SizeType a_size)
		{
			m_vector.resize(a_size);
		}

		constexpr void
		Shrink()
		{
			m_vector.shrink_to_fit();
		}

		[[maybe_unused]]
		constexpr ElementType*
		Add(const ElementType& a_element)
		{
			m_vector.push_back(a_element);
			return &m_vector.back();
		}

		[[maybe_unused]]
		constexpr ElementType*
		Add(ElementType&& a_element)
		{
			m_vector.push_back(std::move(a_element));
			return &m_vector.back();
		}

		constexpr void
		Clear() noexcept
		{
			m_vector.clear();
		}

		constexpr bool
		Contains(const ElementType& a_element) const
		{
			auto iter = std::find(m_vector.begin(), m_vector.end(), a_element);
			return iter != m_vector.end();
		}

	private:
		std::vector<ElementType> m_vector;

	protected:
		using VectorType = decltype(m_vector);
	};

	template<typename TElement>
	TElement*
	begin(Array<TElement>& a_array)
	{
		return a_array.Data();
	}

	template<typename TElement>
	const TElement*
	begin(const Array<TElement>& a_array)
	{
		return a_array.Data();
	}

	template<typename TElement>
	TElement*
	end(Array<TElement>& a_array)
	{
		return a_array.Data() + a_array.Size();
	}

	template<typename TElement>
	const TElement*
	end(const Array<TElement>& a_array)
	{
		return a_array.Data() + a_array.Size();
	}

	template<typename TElement>
	class ArrayProxy : public Array<TElement>
	{
	public:
		using ArrayType = Array<TElement>;
		using ElementType = typename ArrayType::ElementType;

		constexpr
		ArrayProxy()
			: ArrayType() {}

		constexpr
		ArrayProxy(const ElementType& a_element)
			: ArrayType()
		{
			this->Add(a_element);
		}

		constexpr
		ArrayProxy(ElementType&& a_element)
			: ArrayType()
		{
			this->Add(std::move(a_element));
		}

		ArrayProxy(const ArrayType& a_array)
			: ArrayType(a_array) {}

		ArrayProxy(ArrayType&& a_vector)
			: ArrayType(std::move(a_vector)) {}

		ArrayProxy(const typename ArrayType::VectorType& a_vector)
			: ArrayType(a_vector) {}

		ArrayProxy(typename ArrayType::VectorType&& a_vector)
			: ArrayType(std::move(a_vector)) {}
	};
}
