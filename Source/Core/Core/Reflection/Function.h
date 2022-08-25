#pragma once

#include <cassert>
#include <functional>

#include "TypeId.h"

namespace Rearm::Reflection
{
	// TODO: Look into pointer-to-member
	/**
	 * \brief Runtime representation of a field member.
	 */
	class Function
	{
	public:
		Function(
			const std::string&         a_name,
			const std::string&         a_displayName,
			const std::string&         a_description,
			std::function<void(void*)> a_function
		)
			: name(a_name),
			  displayName(a_displayName),
			  description(a_description),
			  function(a_function) {}

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

		const std::function<void(void*)>&
		GetFunction() const
		{
			return function;
		}

		// The actual name of the field in the source code
		std::string name;

		// The name to display to the user in visual applications. Can be user-defined.
		std::string displayName;

		// The description of the field. Optionally user-defined, else an empty string.
		std::string description;

		std::function<void(void*)> function;
	};
}