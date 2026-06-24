#pragma once

namespace Spyll
{
	class ReflectionParser;

	class ReflectionEmitter
	{
	public:
		ReflectionEmitter() {}

		void
		Compile(const ReflectionParser* a_parser);

		void
		Emit();
	};
}
