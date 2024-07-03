#include "pch.h"
#include "Assembly.h"

namespace Oyl::Reflection
{
	bool
	Assembly::RegisterType(std::shared_ptr<Type>& a_type)
	{
		std::shared_ptr<Assembly> assembly;
		for (auto& assemblyPtr : Assemblies())
		{
			if (assemblyPtr.get() == this)
			{
				assembly = assemblyPtr;
			}
			
			auto& ownedTypes = assemblyPtr->m_ownedTypes;
			auto iter = std::find_if(
				ownedTypes.begin(),
				ownedTypes.end(),
				[a_type](auto const& a_value)
				{
					return *a_value == *a_type;
				}
			);

			if (iter != ownedTypes.end())
			{
				OYL_LOG_ERROR(
					"Type \"{}\" trying to be registered under assembly \"{}\" is already registered to assembly \"{}\"",
					a_type->FullName(),
					m_name,
					assemblyPtr->m_name
				);
				return false;
			}
		}

		m_ownedTypes.push_back(a_type);
		a_type->m_assembly = assembly;

		return true;
	}

	bool
	Assembly::UnRegisterType(std::shared_ptr<Type>& a_type)
	{
		auto iter = std::find_if(
			m_ownedTypes.begin(),
			m_ownedTypes.end(),
			[a_type](auto const& a_value)
			{
				return *a_value == *a_type;
			}
		);

		if (iter == m_ownedTypes.end())
		{
			return false;
		}

		m_ownedTypes.erase(iter);
		a_type->m_assembly.reset();
		return true;
	}
}
