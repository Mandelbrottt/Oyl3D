#pragma once

namespace Spyll
{
	class ReflectionParser;

	using OnSourceParsedFn = void(*)(const ReflectionParser*);

	struct ReflectionParserOptions
	{
		OnSourceParsedFn onSourceParsedCallback = nullptr;
	};
}
