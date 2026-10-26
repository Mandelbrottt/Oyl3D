#pragma once

#include <bit>

#include <Core/Containers/SharedPtr.h>
#include <Core/Interfaces/IDestroyable.h>
#include <Core/Preprocessor/Common.h>
#include <Core/Types/TypeTraits.h>

namespace Oyl
{
	template<typename TId>
	class IDeviceObject : public IDestroyable
	{
	protected:
		virtual
		~IDeviceObject() = 0;

		IDeviceObject() = default;
		DEFAULT_MOVE(IDeviceObject);

	public:
		using IdTy = TId;

		NO_COPY(IDeviceObject);

		TId
		GetId() const
		{
			return GetIdImpl();
		}

		explicit
		operator TId() const { return GetId(); }

		TId
		operator *() const { return GetId(); }

	protected:
		virtual
		TId
		GetIdImpl() const = 0;
	};

	template<typename TObject>
	class IObjectHandle final : public SharedPtr<TObject>
	{
		IObjectHandle() = default;

	public:
		IObjectHandle(nullptr_t) {}

		IObjectHandle(const IObjectHandle& a_other) = default;
		IObjectHandle&
		operator =(const IObjectHandle& a_other) = default;

		IObjectHandle(IObjectHandle&& a_other) noexcept = default;
		IObjectHandle&
		operator =(IObjectHandle&& a_other) noexcept = default;

		using SharedPtr<TObject>::SharedPtr;

		operator TObject&() { return *this->Get(); }
		operator const TObject&() const { return *this->Get(); }
	};

	template<typename TId>
	IDeviceObject<TId>::~IDeviceObject() {}

	template<typename>
	struct IDeviceObjectId
	{
		constexpr
		IDeviceObjectId() = default;

		constexpr
		IDeviceObjectId(nullptr_t) {}

		DEFAULT_COPY(IDeviceObjectId);
		DEFAULT_MOVE(IDeviceObjectId);

		constexpr
		operator bool() const { return m_id != nullptr; }

		constexpr bool
		operator==(IDeviceObjectId a_other) const { return m_id == a_other.m_id; }

		constexpr bool
		operator!=(IDeviceObjectId a_other) const { return !(m_id == a_other.m_id); }

		constexpr bool
		operator==(nullptr_t) const { return m_id == nullptr; }

		constexpr bool
		operator!=(nullptr_t) const { return m_id != nullptr; }

	protected:
		struct Id;
		Id* m_id = nullptr;
	};

	template<typename TId, typename TOtherId>
		requires (sizeof(TId) == sizeof(TOtherId)
		          && std::is_trivially_destructible_v<TId>
		          && std::is_trivially_destructible_v<TOtherId>)
	struct IConvertibleDeviceObjectId
	{
		constexpr
		IConvertibleDeviceObjectId() = default;

		constexpr
		IConvertibleDeviceObjectId(nullptr_t) {}

		constexpr
		IConvertibleDeviceObjectId(TId a_other)
		{
			*this = std::bit_cast<IConvertibleDeviceObjectId>(a_other);
		}

		constexpr
		operator TId() const
		{
			return m_id;
		}

		constexpr
		IConvertibleDeviceObjectId(TOtherId a_libType)
		{
			m_id = std::bit_cast<TId>(a_libType);
		}

		constexpr
		operator TOtherId() const
		{
			return std::bit_cast<TOtherId>(m_id);
		}

	private:
		TId m_id;
	};
}
