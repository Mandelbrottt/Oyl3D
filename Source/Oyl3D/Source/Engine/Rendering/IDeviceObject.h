#pragma once

#include <bit>

#include <Core/Interfaces/IDestroyable.h>
#include <Core/Preprocessor/Common.h>

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
		NO_COPY(IDeviceObject);

		TId
		GetId() const
		{
			return GetIdImpl();
		}

		operator TId() const
		{
			return GetId();
		}

	protected:
		virtual
		TId
		GetIdImpl() const = 0;
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
