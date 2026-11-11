#pragma once

namespace Oyl
{
	template<typename TValue>
	class Optional
	{
	public:
		using ValueTy = TValue;

		Optional() = default;

		~Optional() noexcept { Reset(); }

		Optional(const Optional& a_rhs)
			requires std::is_copy_assignable_v<TValue>
		{
			*this = a_rhs;
		}

		Optional&
		operator =(const Optional& a_rhs)
			requires std::is_copy_assignable_v<TValue>
		{
			if (!a_rhs.HasValue())
				Reset();
			else
				Assign(a_rhs.Value());
			return *this;
		}

		Optional(Optional&& a_rhs) noexcept
			requires std::is_move_assignable_v<TValue>
		{
			this = a_rhs;
		}

		Optional&
		operator =(Optional&& a_rhs) noexcept
			requires std::is_move_assignable_v<TValue>
		{
			if (!a_rhs.HasValue())
				Reset();
			else
				Assign(std::move(a_rhs.Value()));
			return *this;
		}

		Optional(const TValue& a_value)
			requires std::is_copy_assignable_v<TValue>
		{
			this = a_value;
		}

		Optional&
		operator =(const TValue& a_value)
			requires std::is_copy_assignable_v<TValue>
		{
			Assign(a_value);
			return *this;
		}

		Optional(TValue&& a_value) noexcept
			requires std::is_move_assignable_v<TValue>
		{
			*this = std::move(a_value);
		}

		Optional&
		operator =(TValue&& a_value) noexcept
			requires std::is_move_assignable_v<TValue>
		{
			Assign(std::move(a_value));
			return *this;
		}

		operator bool() const noexcept { return HasValue(); }

		bool
		HasValue() const noexcept { return m_hasValue; }

		void
		Reset()
		{
			if (!m_hasValue)
				return;

			if constexpr (!std::is_trivially_destructible_v<TValue>)
				StorageAsPtr()->~TValue();
			std::memset(m_storage, 0, sizeof(m_storage));
			m_hasValue = false;
		}

		void
		Assign(const TValue& a_value)
			requires std::is_copy_assignable_v<TValue>
		{
			if (m_hasValue)
				*StorageAsPtr() = a_value;
			else
				new(StorageAsPtr()) TValue(a_value);
			m_hasValue = true;
		}

		void
		Assign(TValue&& a_value) noexcept
			requires std::is_move_assignable_v<TValue>
		{
			if (m_hasValue)
				*StorageAsPtr() = std::move(a_value);
			else
				new(StorageAsPtr()) TValue(std::move(a_value));
			m_hasValue = true;
		}

		TValue&
		Value() noexcept { return *StorageAsPtr(); }

		const TValue&
		Value() const noexcept { return const_cast<Optional*>(this)->Value(); }

		TValue&
		operator *() noexcept { return Value(); }

		const TValue&
		operator *() const noexcept { return const_cast<Optional*>(this)->operator*(); }

		TValue*
		operator ->() noexcept { return StorageAsPtr(); }

		const TValue*
		operator ->() const noexcept { return const_cast<Optional*>(this)->operator->(); }

	private:
		TValue*
		StorageAsPtr() noexcept { return std::launder(reinterpret_cast<TValue*>(m_storage)); }

		alignas(TValue)
		byte m_storage[sizeof(TValue)] {};

		bool m_hasValue = false;
	};
}
