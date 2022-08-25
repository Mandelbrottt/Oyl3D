#pragma once

#include <cassert>

#include "TypeId.h"

namespace Rearm::Reflection
{
	enum class FieldFlags : uint32_t
	{
		None = 0,

		Static = 0x01,
		//FfNonStatic = 0x02,
		Private = 0x04,
		Protected = 0x08,
		//FfPrivate   = 0x10,
	};

	// TODO: Look into pointer-to-member
	/**
	 * \brief Runtime representation of a field member.
	 */
	class Field
	{
	public:
		Field(
			const std::string& a_name,
			const std::string& a_displayName,
			const std::string& a_description,
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

		const std::string&
		GetName() const
		{
			return name;
		}

		const std::string&
		GetDisplayName() const
		{
			return displayName;
		}

		const std::string&
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
		const void*
		GetValue(const TContaining& a_obj) const;

		/**
		 * \brief Get a read-only reference to the field for the given object.
		 * \tparam TField The type of the field.
		 * \tparam TContaining The type that contains the field (can be void*).
		 * \param a_obj The object to get the value for.
		 * \return A pointer to the instance of the field on object obj.
		 */
		template<typename TField, typename TContaining>
		const TField&
		GetValue(const TContaining& a_obj) const;

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
		void
		SetValue(const TContaining& a_obj, const void* a_incomingValue, size_t a_incomingSize) const;

		/**
		 * \brief Set the value for the field for the given object
		 * \tparam TContaining The type that contains the field (can be void*)
		 * \tparam TField The type of the field
		 * \param a_obj The object to set the value for
		 * \param a_value A reference to the value to be copied.
		 */
		template<typename TContaining, typename TField>
		void
		SetValue(const TContaining& a_obj, const TField& a_value) const;

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

	/**
	 * \brief The display name of a reflected member
	 */
	struct Name
	{
		constexpr
		Name()
			: c_str("") {}

		constexpr
		Name(const char* a_value)
			: c_str(a_value) {}

		const char* c_str;
	};

	/**
	 * \brief The description of a reflected member
	 */
	struct Description
	{
		constexpr
		Description()
			: c_str("") {}

		constexpr
		Description(const char* a_value)
			: c_str(a_value) {}

		const char* c_str;
	};
}

#include "Field.inl"
