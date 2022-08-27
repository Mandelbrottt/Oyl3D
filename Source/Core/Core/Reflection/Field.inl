#pragma once

namespace Rearm::Reflection
{
	template<typename TContaining>
	const void*
	Field::GetValue(const TContaining& a_obj) const
	{
	#ifndef REARM_RELEASE
		Type& containingType     = Type::Get<TContaining>();
		Type* thisContainingType = Type::TryGet(this->m_containingTypeId);
	
		bool notNull       = thisContainingType;
		bool isConvertible = thisContainingType->IsConvertibleFrom(&containingType);
		bool isVoid        = containingType.GetTypeId() == GetTypeId<void>();
	
		assert(notNull && (isConvertible || isVoid));
	#endif
		
		uintptr_t ptrToMember;

		if constexpr (std::is_pointer_v<TContaining>)
			ptrToMember = reinterpret_cast<uintptr_t>(a_obj);
		else
			ptrToMember = reinterpret_cast<uintptr_t>(&a_obj);

		ptrToMember += m_offset;

		return reinterpret_cast<void*>(ptrToMember);
	}

	template<typename TField, typename TContaining>
	const TField&
	Field::GetValue(const TContaining& a_obj) const
	{
	#ifndef REARM_RELEASE
		const Type& incomingType = Type::Get<TField>();
		const Type* thisType     = Type::TryGet(this->m_fieldTypeId);

		assert(thisType && thisType->IsConvertibleTo(&incomingType));
	#endif

		const void* result = GetValue(a_obj);

		return *static_cast<const TField*>(result);
	}

	template<typename TContaining>
	void
	Field::SetValue(const TContaining& a_obj, const void* a_incomingValue, size_t a_incomingSize) const
	{
	#ifndef REARM_RELEASE
		const Type& containingType     = Type::Get<TContaining>();
		const Type* thisContainingType = Type::TryGet(this->m_containingTypeId);

		bool notNull       = thisContainingType;
		bool isConvertible = thisContainingType->IsConvertibleFrom(&containingType);
		bool isVoid        = containingType.GetTypeId() == GetTypeId<void>();
		bool sizeEqual     = a_incomingSize == m_size;

		assert(notNull && (isConvertible || isVoid) && sizeEqual);
	#endif

		uintptr_t ptrToMember;

		if constexpr (std::is_pointer_v<TContaining>)
			ptrToMember = reinterpret_cast<uintptr_t>(a_obj);
		else
			ptrToMember = reinterpret_cast<uintptr_t>(&a_obj);

		ptrToMember += m_offset;

		std::memcpy(reinterpret_cast<void*>(ptrToMember), a_incomingValue, a_incomingSize);
	}

	template<typename TContaining, typename TField>
	void
	Field::SetValue(const TContaining& a_obj, const TField& a_value) const
	{
	#ifndef REARM_RELEASE
		const Type& containingType     = Type::Get<TContaining>();
		const Type* thisContainingType = Type::TryGet(this->m_containingTypeId);

		bool notNull       = thisContainingType;
		bool isConvertible = thisContainingType->IsConvertibleFrom(&containingType);
		bool isVoid        = containingType.GetTypeId() == GetTypeId<void>();

		// TODO: Do more elaborate type checks
		bool sizeEqual = sizeof(TField) == m_size;

		assert(notNull && (isConvertible || isVoid) && sizeEqual);
	#endif
		
		uintptr_t ptrToMember;

		if constexpr (std::is_pointer_v<TContaining>)
			ptrToMember = reinterpret_cast<uintptr_t>(a_obj);
		else
			ptrToMember = reinterpret_cast<uintptr_t>(&a_obj);

		ptrToMember += m_offset;

		TField& refToMember = *std::launder(reinterpret_cast<TField*>(ptrToMember));

		// Do this instead of calling other SetValue function to call copy constructor
		refToMember = a_value;
	}
}
