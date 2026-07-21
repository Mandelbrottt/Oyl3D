#include "Type.h"

#include "Assembly.h"
#include "ReflectionFactory.h"

namespace Oyl::Reflection
{
	struct Type::Impl
	{
		TypeId typeId = TypeId::Null;

		uint32_t size = 0;

		std::string_view name;
		std::string_view qualifiedName;

		bool isPrimitive = false;

		Assembly* assembly = nullptr;

		std::vector<Type*> baseTypes;

		std::vector<Variable*> variables;
		std::vector<Field*> fields;

		std::vector<Function*> functions;
		std::vector<Method*> methods;
	};

	Type::Type(const Internal::TypeParams& a_params)
	{
		m_impl = new Impl;

		m_impl->assembly = a_params.assembly;

		m_impl->typeId = a_params.typeId;
		m_impl->size = a_params.size;

		m_impl->qualifiedName = a_params.qualifiedName;
		m_impl->name = a_params.name;

		m_impl->isPrimitive = false;
	}

	Type::~Type()
	{
		delete m_impl;
		m_impl = nullptr;
	}

	Type*
	Type::Get(std::string_view a_name)
	{
		auto& assemblies = Assembly::GetAssemblies();

		for (Assembly* assembly : assemblies)
		{
			if (Type* type = assembly->GetType(a_name))
			{
				return type;
			}
		}

		return nullptr;
	}

	TypeId
	Type::GetTypeId() const
	{
		return m_impl->typeId;
	}

	std::string_view
	Type::GetName() const
	{
		return m_impl->name;
	}

	std::string_view
	Type::GetQualifiedName() const
	{
		return m_impl->qualifiedName;
	}

	uint32_t
	Type::GetSize() const
	{
		return m_impl->size;
	}

	bool
	Type::IsPrimitive() const
	{
		return m_impl->isPrimitive;
	}

	const std::vector<Type*>&
	Type::GetBaseTypes() const
	{
		return m_impl->baseTypes;
	}

	const std::vector<Variable*>&
	Type::GetVariables() const
	{
		return m_impl->variables;
	}

	const std::vector<Field*>&
	Type::GetFields() const
	{
		return m_impl->fields;
	}

	const std::vector<Function*>&
	Type::GetFunctions() const
	{
		return m_impl->functions;
	}

	const std::vector<Method*>&
	Type::GetMethods() const
	{
		return m_impl->methods;
	}

	void
	Type::AddBaseType(Type* a_type)
	{
		m_impl->baseTypes.emplace_back(a_type);
	}

	void
	Type::AddVariable(Variable* a_variable)
	{
		m_impl->variables.emplace_back(a_variable);
	}

	void
	Type::AddField(Field* a_field)
	{
		m_impl->fields.emplace_back(a_field);
	}

	void
	Type::AddFunction(Function* a_function)
	{
		m_impl->functions.emplace_back(a_function);
	}

	void
	Type::AddMethod(Method* a_method)
	{
		m_impl->methods.emplace_back(a_method);
	}
}
