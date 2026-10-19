#pragma once

#include <memory>

#include <Core/Preprocessor/Common.h>
#include <Core/Types/TypeTraits.h>

namespace Oyl
{
	template<typename TPointee>
	class UniquePtr
	{
		template<typename UPointee>
		friend class UniquePtr;

	public:
		using PointeeTy = TPointee;

		UniquePtr() = default;
		UniquePtr(nullptr_t) {}

		NO_COPY(UniquePtr);

		UniquePtr(UniquePtr&& a_other) noexcept
		{
			*this = std::move(a_other);
		}

		UniquePtr&
		operator =(UniquePtr&& a_other) noexcept
		{
			if (this != &a_other)
			{
				Swap(std::move(a_other));
			}
			return *this;
		}

		template<typename UPointee>
			requires Traits::ConvertibleTo<UPointee*, PointeeTy*>
		UniquePtr(UniquePtr<UPointee>&& a_other) noexcept
		{
			*this = std::move(a_other);
		}

		template<typename UPointee>
			requires Traits::ConvertibleTo<UPointee*, PointeeTy*>
		UniquePtr&
		operator =(UniquePtr<UPointee>&& a_other) noexcept
		{
			m_unique_ptr = std::move(a_other.m_unique_ptr);
			return *this;
		}

		template<typename UPointee>
			requires Traits::ConvertibleTo<UPointee*, PointeeTy*>
		UniquePtr(UPointee* a_pointee)
		{
			m_unique_ptr = std::unique_ptr<PointeeTy>(a_pointee);
		}

		operator bool() const
		{
			return (bool) m_unique_ptr;
		}

		template<typename UPointee>
			requires Traits::ConvertibleTo<UPointee*, PointeeTy*>
		friend
		bool
		operator ==(UniquePtr&& a_lhs, UniquePtr<UPointee>&& a_rhs)
		{
			return a_lhs.Get() == a_rhs.Get();
		}

		template<typename UPointee>
			requires Traits::ConvertibleTo<UPointee*, PointeeTy*>
		friend
		bool
		operator !=(UniquePtr<UPointee>&& a_lhs, UniquePtr&& a_rhs)
		{
			return !(a_lhs == a_rhs);
		}

		template<typename UPointee>
			requires Traits::ConvertibleTo<UPointee*, PointeeTy*>
		friend
		bool
		operator ==(UniquePtr<UPointee>&& a_lhs, UniquePtr&& a_rhs)
		{
			return a_lhs.Get() == a_rhs.Get();
		}

		template<typename UPointee>
			requires Traits::ConvertibleTo<UPointee*, PointeeTy*>
		friend
		bool
		operator !=(UniquePtr&& a_lhs, UniquePtr<UPointee>&& a_rhs)
		{
			return !(a_lhs == a_rhs);
		}

		PointeeTy*
		Get() const noexcept { return m_unique_ptr.get(); }

		PointeeTy*
		operator ->() const noexcept { return Get(); }

		PointeeTy&
		operator *() const noexcept { return *Get(); }

		void
		Swap(UniquePtr&& a_other)
		{
			std::swap(m_unique_ptr, a_other.m_unique_ptr);
		}

		void
		Reset()
		{
			m_unique_ptr.reset();
		}

		template<typename UPointee>
			requires Traits::ConvertibleTo<UPointee*, PointeeTy*>
		void
		Reset(UPointee* a_newPointee)
		{
			m_unique_ptr.reset(a_newPointee);
		}

		PointeeTy*
		Release()
		{
			return m_unique_ptr.release();
		}

		template<typename... TArgs>

		[[nodiscard]]
		static
		UniquePtr
		Make(TArgs&&... a_args)
		{
			UniquePtr ptr;
			ptr.m_unique_ptr = std::make_unique<PointeeTy>(std::forward<TArgs>(a_args)...);
			return ptr;
		}

	private:
		std::unique_ptr<PointeeTy> m_unique_ptr;
	};

	template<typename TPointee, typename... TArgs>
	[[nodiscard]]
	UniquePtr<TPointee>
	MakeUniquePtr(TArgs&&... a_args)
	{
		return UniquePtr<TPointee>::Make(std::forward<TArgs>(a_args)...);
	}
}
