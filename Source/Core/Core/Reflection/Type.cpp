#include "pch.h"
#include "Type.h"

namespace Oyl::Reflection
{
	Type::~Type()
	{
		Factory()->~GenericFactory();
		std::memset(&m_factoryData, 0, sizeof(m_factoryData));
	}

	bool
	Type::IsConvertibleTo(Oyl::TypeId a_typeId) const
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
	Type::IsConvertibleTo(Type const* a_type) const
	{
		return IsConvertibleTo(a_type->TypeId());
	}

	bool
	Type::IsConvertibleTo(Type const& a_type) const
	{
		return IsConvertibleTo(a_type.TypeId());
	}

	bool
	Type::IsConvertibleFrom(Oyl::TypeId a_typeId) const
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
	Type::IsConvertibleFrom(Type const* a_type) const
	{
		return IsConvertibleFrom(a_type->TypeId());
	}

	bool
	Type::IsConvertibleFrom(Type const& a_type) const
	{
		return IsConvertibleFrom(a_type.TypeId());
	}

	Type* Type::TryGet(Oyl::TypeId a_id) noexcept
	{
		Type* result = nullptr;
		auto& types  = Types();
		if (auto iter = types.find(a_id); iter != types.end())
		{
			result = &iter->second;
		}

		return result;
	}

	Type* Type::TryGet(std::string_view a_fullyQualifiedTypeName) {
		Type* result = nullptr;
		auto& types  = Types();
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

	void Type::ProcessName(std::type_info const& a_typeInfo) {
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
