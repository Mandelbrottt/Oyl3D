#pragma once

namespace Spyll::Reflection
{
	enum class AccessSpecifier : uint8_t
	{
		None = 0,
		Public,
		Protected,
		Private
	};
	
	enum class ConstructorType : uint8_t
	{
		None,
		Default,
		Copy,
		Move
	};
}