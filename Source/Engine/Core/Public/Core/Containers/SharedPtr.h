#pragma once

#include <memory>

#include "Core/Types/TypeId.h"

namespace Oyl
{
	template<typename TPointee>
	class SharedPtr
	{
		template<typename UPointee>
		friend class SharedPtr;

	public:
		using PointeeTy = TPointee;

		SharedPtr() = default;
		SharedPtr(nullptr_t) {}

		SharedPtr(const SharedPtr& a_other)
		{
			*this = a_other;
		}

		SharedPtr&
		operator =(const SharedPtr& a_other)
		{
			m_shared_ptr = a_other.m_shared_ptr;
			return *this;
		}

		SharedPtr(SharedPtr&& a_other) noexcept
		{
			*this = std::move(a_other);
		}

		SharedPtr&
		operator =(SharedPtr&& a_other) noexcept
		{
			if (this != &a_other)
			{
				Swap(std::move(a_other));
			}
			return *this;
		}

		template<typename UPointee>
			requires Traits::ConvertibleTo<UPointee*, PointeeTy*>
		SharedPtr(const SharedPtr<UPointee>& a_other)
		{
			*this = a_other;
		}

		template<typename UPointee>
			requires Traits::ConvertibleTo<UPointee*, PointeeTy*>
		SharedPtr&
		operator =(const SharedPtr<UPointee>& a_other)
		{
			m_shared_ptr = a_other.m_shared_ptr;
			return *this;
		}

		template<typename UPointee>
			requires Traits::ConvertibleTo<UPointee*, PointeeTy*>
		SharedPtr(SharedPtr<UPointee>&& a_other) noexcept
		{
			*this = std::move(a_other);
		}

		template<typename UPointee>
			requires Traits::ConvertibleTo<UPointee*, PointeeTy*>
		SharedPtr&
		operator =(SharedPtr<UPointee>&& a_other) noexcept
		{
			m_shared_ptr = std::move(a_other.m_shared_ptr);
			return *this;
		}

		template<typename UPointee>
			requires Traits::ConvertibleTo<UPointee*, PointeeTy*>
		explicit
		SharedPtr(UPointee* a_pointee)
		{
			m_shared_ptr = std::shared_ptr(a_pointee);
		}

		operator bool() const
		{
			return (bool) m_shared_ptr;
		}

		template<typename UPointee>
			requires Traits::ConvertibleTo<UPointee*, PointeeTy*>
		friend
		bool
		operator ==(SharedPtr&& a_lhs, SharedPtr<UPointee>&& a_rhs)
		{
			return a_lhs.Get() == a_rhs.Get();
		}

		template<typename UPointee>
			requires Traits::ConvertibleTo<UPointee*, PointeeTy*>
		friend
		bool
		operator !=(SharedPtr&& a_lhs, SharedPtr<UPointee>&& a_rhs)
		{
			return !(a_lhs == a_rhs);
		}

		template<typename UPointee>
			requires Traits::ConvertibleTo<UPointee*, PointeeTy*>
		friend
		bool
		operator ==(SharedPtr<UPointee>&& a_lhs, SharedPtr&& a_rhs)
		{
			return a_lhs.Get() == a_rhs.Get();
		}

		template<typename UPointee>
			requires Traits::ConvertibleTo<UPointee*, PointeeTy*>
		friend
		bool
		operator !=(SharedPtr<UPointee>&& a_lhs, SharedPtr&& a_rhs)
		{
			return !(a_lhs == a_rhs);
		}

		PointeeTy*
		Get() const noexcept { return m_shared_ptr.get(); }

		PointeeTy*
		operator ->() const noexcept { return Get(); }

		PointeeTy&
		operator *() const noexcept { return *Get(); }

		void
		Swap(SharedPtr&& a_other) noexcept
		{
			std::swap(m_shared_ptr, a_other.m_shared_ptr);
		}

		void
		Reset()
		{
			m_shared_ptr.reset();
		}

		template<typename UPointee>
			requires Traits::ConvertibleTo<UPointee*, PointeeTy*>
		void
		Reset(UPointee* a_newPointee)
		{
			m_shared_ptr.reset(a_newPointee);
		}

		template<typename... TArgs>
		[[nodiscard]]
		static
		SharedPtr
		Make(TArgs&&... a_args)
		{
			SharedPtr ptr;
			ptr.m_shared_ptr = std::make_shared<PointeeTy>(std::forward<TArgs>(a_args)...);
			return ptr;
		}

	private:
		std::shared_ptr<PointeeTy> m_shared_ptr;
	};

	template<typename TPointee, typename... TArgs>
	[[nodiscard]]
	SharedPtr<TPointee>
	MakeSharedPtr(TArgs&&... a_args)
	{
		return SharedPtr<TPointee>::Make(std::forward<TArgs>(a_args)...);
	}
}
