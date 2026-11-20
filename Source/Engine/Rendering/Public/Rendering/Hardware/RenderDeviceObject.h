#pragma once

#include <bit>

#include <Core/Containers/SharedPtr.h>
#include <Core/Interfaces/IDestroyable.h>
#include <Core/Preprocessor/Common.h>
#include <Core/Types/TypeTraits.h>

namespace Oyl
{
	template<typename TId>
	class IRenderDeviceObject : public IDestroyable
	{
	protected:
		virtual
		~IRenderDeviceObject() = 0;

		IRenderDeviceObject() = default;
		DEFAULT_MOVE(IRenderDeviceObject);

	public:
		using IdTy = TId;

		NO_COPY(IRenderDeviceObject);

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
	class IRenderDeviceObjectHandle final : public SharedPtr<TObject>
	{
		IRenderDeviceObjectHandle() = default;

	public:
		IRenderDeviceObjectHandle(nullptr_t) {}

		IRenderDeviceObjectHandle(const IRenderDeviceObjectHandle& a_other) = default;
		IRenderDeviceObjectHandle&
		operator =(const IRenderDeviceObjectHandle& a_other) = default;

		IRenderDeviceObjectHandle(IRenderDeviceObjectHandle&& a_other) noexcept = default;
		IRenderDeviceObjectHandle&
		operator =(IRenderDeviceObjectHandle&& a_other) noexcept = default;

		using SharedPtr<TObject>::SharedPtr;

		operator TObject&() { return *this->Get(); }
		operator const TObject&() const { return *this->Get(); }
	};

	template<typename TId>
	IRenderDeviceObject<TId>::~IRenderDeviceObject() {}

	template<typename>
	struct IRenderDeviceObjectId
	{
		constexpr
		IRenderDeviceObjectId() = default;

		constexpr
		IRenderDeviceObjectId(nullptr_t) {}

		DEFAULT_COPY(IRenderDeviceObjectId);
		DEFAULT_MOVE(IRenderDeviceObjectId);

		constexpr
		operator bool() const { return m_id != nullptr; }

		constexpr bool
		operator==(IRenderDeviceObjectId a_other) const { return m_id == a_other.m_id; }

		constexpr bool
		operator!=(IRenderDeviceObjectId a_other) const { return !(m_id == a_other.m_id); }

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
	struct IConvertibleRenderDeviceObjectId
	{
		constexpr
		IConvertibleRenderDeviceObjectId() = default;

		constexpr
		IConvertibleRenderDeviceObjectId(nullptr_t) {}

		constexpr
		IConvertibleRenderDeviceObjectId(TId a_other)
		{
			*this = std::bit_cast<IConvertibleRenderDeviceObjectId>(a_other);
		}

		constexpr
		operator TId() const
		{
			return m_id;
		}

		constexpr
		IConvertibleRenderDeviceObjectId(TOtherId a_libType)
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
