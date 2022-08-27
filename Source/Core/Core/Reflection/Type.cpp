#include "pch.h"

#include "Type.h"

namespace Rearm::Reflection
{
	Type::types_container_t Type::s_typesContainer;

	const Field*
	Type::GetField(std::string_view a_fieldName) const
	{
		auto predicate = [&a_fieldName](const Field& a_field)
		{
			return a_field.Name() == a_fieldName;
		};

		auto iter = std::find_if(m_instanceFields.begin(), m_instanceFields.end(), predicate);

		if (iter == m_instanceFields.end())
		{
			return nullptr;
		}

		const auto& field = *iter;
		return &field;
	}

	bool
	Type::IsConvertibleTo(TypeId a_typeId) const
	{
		if (a_typeId == TypeId::Null)
		{
			return false;
		}

		if (m_typeId == a_typeId)
		{
			return true;
		}

		Type* parent = TryGet(m_baseTypeId);

		if (parent)
		{
			return parent->IsConvertibleTo(a_typeId);
		}

		return false;
	}

	bool
	Type::IsConvertibleTo(const Type* a_type) const
	{
		return IsConvertibleTo(a_type->GetTypeId());
	}

	bool
	Type::IsConvertibleTo(const Type& a_type) const
	{
		return IsConvertibleTo(a_type.GetTypeId());
	}

	bool
	Type::IsConvertibleFrom(TypeId a_typeId) const
	{
		if (a_typeId == TypeId::Null)
		{
			return false;
		}

		if (m_typeId == a_typeId)
		{
			return true;
		}

		// Check if the other type is convertible to this type
		Type* otherType = TryGet(a_typeId);

		if (otherType)
		{
			return otherType->IsConvertibleTo(m_typeId);
		}

		return false;
	}

	bool
	Type::IsConvertibleFrom(const Type* a_type) const
	{
		return IsConvertibleFrom(a_type->GetTypeId());
	}

	bool
	Type::IsConvertibleFrom(const Type& a_type) const
	{
		return IsConvertibleFrom(a_type.GetTypeId());
	}

	const Type*
	Type::TryGet(TypeId a_id) noexcept
	{
		Type* result = nullptr;
		auto& types  = s_typesContainer;
		if (auto iter = types.find(a_id); iter != types.end())
		{
			result = &iter->second;
		}

		return result;
	}

	const Type*
	Type::TryGet(const std::string_view& a_fullyQualifiedTypeName)
	{
		Type* result = nullptr;
		auto& types  = s_typesContainer;
		for (auto& [typeId, type] : types)
		{
			if (a_fullyQualifiedTypeName.compare(type.FullName()) == 0)
			{
				result = &type;
				break;
			}
		}
		return result;
	}

	void
	Type::ProcessName(const std::type_info& a_typeInfo)
	{
		const char* typeName = a_typeInfo.name();

		// type_info.name() is implementation specific
	#if defined _MSC_BUILD
		// for user types, MSVC uses the format [class, struct] [<namespace>::]<classname> [cv and pointer status]
		// ie. "Type const*" is "class Next::Reflection::Type const * __ptr64" in x86_64
		const char* classString     = strstr(typeName, "class");
		const char* structString    = strstr(typeName, "struct");
		const char* firstWhitespace = strchr(typeName, ' ');

		if (!(classString || structString))
		{
			std::ptrdiff_t difference;

			// Assume this is a primitive type
			if (firstWhitespace)
			{
				// Oh wow, const char* - const char* = long long, who knew?
				difference = (firstWhitespace + 1) - typeName;
			} else
			{
				difference = strlen(typeName);
			}

			m_name     = std::string(typeName, difference);
			m_fullName = m_name;

			return;
		}

		// This probably shouldn't be an assert but im lazy
		// There should always be whitespace in a user-defined-class name
		assert(firstWhitespace);

		const char* fullNameStart = firstWhitespace + 1;

		const char* shortNameStart = fullNameStart;

		// Find the last instance of the scope operator
		while (true)
		{
			const char* find = strstr(shortNameStart, "::");
			if (!find)
			{
				break;
			}
			shortNameStart = find + 2;
		}

		const char* nextWhitespace = strchr(shortNameStart, ' ');

		std::ptrdiff_t fullNameLength;
		std::ptrdiff_t shortNameLength;
		if (nextWhitespace)
		{
			fullNameLength  = nextWhitespace - fullNameStart;
			shortNameLength = nextWhitespace - shortNameStart;
		} else
		{
			fullNameLength  = strlen(fullNameStart);
			shortNameLength = strlen(shortNameStart);
		}

		m_name     = std::string(shortNameStart, shortNameLength);
		m_fullName = std::string(fullNameStart, fullNameLength);
	#endif
	}
}
