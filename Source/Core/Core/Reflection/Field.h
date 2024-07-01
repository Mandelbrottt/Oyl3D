#pragma once

#include "Core/Types/TypeId.h"

namespace Oyl::Reflection
{
	class Type;
	
	enum class FieldFlags : uint32_t
	{
		None = 0,

		Static = 0x01,
		//FfNonStatic = 0x02,
		Private = 0x04,
		Protected = 0x08,
		//FfPrivate   = 0x10,
	};

	inline
	FieldFlags
	operator |(FieldFlags a_lhs, FieldFlags a_rhs)
	{
		auto lhs = static_cast<std::underlying_type_t<FieldFlags>>(a_lhs);
		auto rhs = static_cast<std::underlying_type_t<FieldFlags>>(a_rhs);
		return static_cast<FieldFlags>(lhs | rhs);
	}
	
	inline
	FieldFlags
	operator &(FieldFlags a_lhs, FieldFlags a_rhs)
	{
		auto lhs = static_cast<std::underlying_type_t<FieldFlags>>(a_lhs);
		auto rhs = static_cast<std::underlying_type_t<FieldFlags>>(a_rhs);
		return static_cast<FieldFlags>(lhs & rhs);
	}

	/**
	 * \brief Runtime representation of a field member.
	 */
	class Field
	{
	public:
		Field(
			std::string const& a_name,
			std::string const& a_displayName,
			std::string const& a_description,
			uint32_t           a_offset,
			uint32_t           a_size,
			TypeId             a_fieldTypeId,
			TypeId             a_containingTypeId,
			FieldFlags         a_flags
		)
			: name(a_name),
			  displayName(a_displayName),
			  description(a_description),
			  offset(a_offset),
			  size(a_size),
			  fieldTypeId(a_fieldTypeId),
			  containingTypeId(a_containingTypeId),
			  flags(a_flags) {}
		
		std::string const&
		GetName() const
		{
			return name;
		}
		
		std::string const&
		GetDisplayName() const
		{
			return displayName;
		}
		
		std::string const&
		GetDescription() const
		{
			return description;
		}
		
		uint32_t
		GetOffset() const
		{
			return offset;
		}
		
		uint32_t
		GetSize() const
		{
			return size;
		}
		
		TypeId
		GetFieldTypeId() const
		{
			return fieldTypeId;
		}
		
		TypeId
		GetContainingTypeId() const
		{
			return containingTypeId;
		}
		
		FieldFlags
		GetFlags() const
		{
			return flags;
		}
		
		/**
		 * \brief Get a read-only pointer to the field for the given object.
		 * \tparam TContaining The type that contains the field (can be void*).
		 * \param a_obj The object to get the value for.
		 * \return A pointer to the instance of the field on object obj.
		 */
		template<typename TContaining
			/*, std::enable_if_t<!std::is_pointer_v<std::remove_pointer_t<TContaining>>, bool> = true*/>
		const void* GetValue(TContaining const& a_obj) const
		{
			Type& containingType     = Type::Get<TContaining>();
			Type* thisContainingType = Type::TryGet(this->containingTypeId);

			bool notNull = thisContainingType;
			bool isConvertible = thisContainingType->IsConvertibleTo(&containingType);
			bool isVoid = containingType.GetTypeId() == GetTypeId<void>();
			
			assert(notNull && (isConvertible || isVoid));

			uintptr_t ptrToMember;

			if constexpr (std::is_pointer_v<TContaining>)
				ptrToMember = reinterpret_cast<uintptr_t>(a_obj);
			else
				ptrToMember = reinterpret_cast<uintptr_t>(&a_obj);

			ptrToMember += offset;

			return reinterpret_cast<void*>(ptrToMember);
		}
		
		/**
		 * \brief Get a read-only reference to the field for the given object.
		 * \tparam TField The type of the field.
		 * \tparam TContaining The type that contains the field (can be void*).
		 * \param a_obj The object to get the value for.
		 * \return A pointer to the instance of the field on object obj.
		 */
		template<typename TField, typename TContaining>
		TField const& GetValue(TContaining const& a_obj) const
		{
			Type& incomingType = Type::Get<TField>();
			Type* thisType     = Type::TryGet(this->fieldTypeId);

			assert(thisType && thisType->IsConvertibleTo(&incomingType));

			const void* result = Field::GetValue(a_obj);

			return *static_cast<const TField*>(result);
		}
		
		/**
		 * \brief Set the value for the field for the given object
		 * \tparam TContaining The type that contains the field (can be void*)
		 * \param a_obj The object to set the value for
		 * \param a_incomingValue A pointer to the value to be copied.
		 * \param a_incomingSize the size in bytes of the incoming value.
		 * \remark Uses memcpy under the hood, so avoid using with non-trivially copyable types.
		 *         This is the caller's responsibility.
		 */
		template<typename TContaining
			/*, std::enable_if_t<!std::is_pointer_v<std::remove_pointer_t<TContaining>>, bool> = true*/>
		void SetValue(TContaining const& a_obj, const void* a_incomingValue, size_t a_incomingSize) const
		{
			Type& containingType     = Type::Get<TContaining>();
			Type* thisContainingType = Type::TryGet(this->containingTypeId);

			bool notNull = thisContainingType;
			bool isConvertible = thisContainingType->IsConvertibleTo(&containingType);
			bool isVoid = containingType.GetTypeId() == GetTypeId<void>();
			bool sizeEqual = a_incomingSize == size;
			
			assert(notNull && (isConvertible || isVoid) && sizeEqual);
			
			uintptr_t ptrToMember;

			if constexpr (std::is_pointer_v<TContaining>)
				ptrToMember = reinterpret_cast<uintptr_t>(a_obj);
			else
				ptrToMember = reinterpret_cast<uintptr_t>(&a_obj);

			ptrToMember += offset;

			std::memcpy(reinterpret_cast<void*>(ptrToMember), a_incomingValue, a_incomingSize);
		}

		/**
		 * \brief Set the value for the field for the given object
		 * \tparam TContaining The type that contains the field (can be void*)
		 * \tparam TField The type of the field
		 * \param a_obj The object to set the value for
		 * \param a_value A reference to the value to be copied.
		 */
		template<typename TContaining, typename TField>
		void SetValue(TContaining const& a_obj, TField const& a_value) const
		{
			Type& containingType     = Type::Get<TContaining>();
			Type* thisContainingType = Type::TryGet(this->containingTypeId);

			bool notNull = thisContainingType;
			bool isConvertible = thisContainingType->IsConvertibleTo(&containingType);
			bool isVoid = containingType.GetTypeId() == GetTypeId<void>();

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
		
		// The actual name of the field in the source code
		std::string name;

		// The name to display to the user in visual applications. Can be user-defined.
		std::string displayName;

		// The description of the field. Optionally user-defined, else an empty string.
		std::string description;

		// The offset in bytes into the containing object that the field resides at.
		uint32_t offset;

		// The size returned by sizeof of the field.
		uint32_t size;

		// The StaticTypeId of the field.
		TypeId fieldTypeId;

		// The StaticTypeId of the object that the field is contained in.
		TypeId containingTypeId;

		// Flags that provide additional information about the field.
		FieldFlags flags = FieldFlags::None;
	};
}
