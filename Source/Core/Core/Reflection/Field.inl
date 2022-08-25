#pragma once

namespace Rearm::Reflection
{
	template<typename TContaining>
	const void*
	Field::GetValue(const TContaining& a_obj) const
	{
		Type& containingType     = Type::Get<TContaining>();
		Type* thisContainingType = Type::TryGet(this->containingTypeId);

		bool notNull       = thisContainingType;
		bool isConvertible = thisContainingType->IsConvertibleTo(&containingType);
		bool isVoid        = containingType.GetTypeId() == GetTypeId<void>();

		assert(notNull && (isConvertible || isVoid));

		uintptr_t ptrToMember;

		if constexpr (std::is_pointer_v<TContaining>)
			ptrToMember = reinterpret_cast<uintptr_t>(a_obj);
		else
			ptrToMember = reinterpret_cast<uintptr_t>(&a_obj);

		ptrToMember += offset;

		return reinterpret_cast<void*>(ptrToMember);
	}

	template<typename TField, typename TContaining>
	const TField&
	Field::GetValue(const TContaining& a_obj) const
	{
		Type& incomingType = Type::Get<TField>();
		Type* thisType     = Type::TryGet(this->fieldTypeId);

		assert(thisType && thisType->IsConvertibleTo(&incomingType));

		const void* result = Field::GetValue(a_obj);

		return *static_cast<const TField*>(result);
	}

	template<typename TContaining>
	void
	Field::SetValue(const TContaining& a_obj, const void* a_incomingValue, size_t a_incomingSize) const
	{
		Type& containingType     = Type::Get<TContaining>();
		Type* thisContainingType = Type::TryGet(this->containingTypeId);

		bool notNull       = thisContainingType;
		bool isConvertible = thisContainingType->IsConvertibleTo(&containingType);
		bool isVoid        = containingType.GetTypeId() == GetTypeId<void>();
		bool sizeEqual     = a_incomingSize == size;

		assert(notNull && (isConvertible || isVoid) && sizeEqual);

		uintptr_t ptrToMember;

		if constexpr (std::is_pointer_v<TContaining>)
			ptrToMember = reinterpret_cast<uintptr_t>(a_obj);
		else
			ptrToMember = reinterpret_cast<uintptr_t>(&a_obj);

		ptrToMember += offset;

		std::memcpy(reinterpret_cast<void*>(ptrToMember), a_incomingValue, a_incomingSize);
	}

	template<typename TContaining, typename TField>
	void
	Field::SetValue(const TContaining& a_obj, const TField& a_value) const
	{
		Type& containingType     = Type::Get<TContaining>();
		Type* thisContainingType = Type::TryGet(this->containingTypeId);

		bool notNull       = thisContainingType;
		bool isConvertible = thisContainingType->IsConvertibleTo(&containingType);
		bool isVoid        = containingType.GetTypeId() == GetTypeId<void>();

		// TODO: Do more elaborate type checks
		bool sizeEqual = sizeof(TField) == size;

		assert(notNull && (isConvertible || isVoid) && sizeEqual);

		uintptr_t ptrToMember;

		if constexpr (std::is_pointer_v<TContaining>)
			ptrToMember = reinterpret_cast<uintptr_t>(a_obj);
		else
			ptrToMember = reinterpret_cast<uintptr_t>(&a_obj);

		ptrToMember += offset;

		TField& refToMember = *std::launder(reinterpret_cast<TField*>(ptrToMember));

		// Do this instead of calling other SetValue function to call copy constructor
		refToMember = a_value;
	}
}
