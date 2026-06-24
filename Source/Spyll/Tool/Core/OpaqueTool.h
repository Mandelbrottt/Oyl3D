#pragma once

#include <string>

#include "ReflectionParseAction.h"

namespace Spyll
{
	class ReflectionGenerator;

	class OpaqueTool
	{
	public:
		OpaqueTool();

		virtual
		~OpaqueTool();

		OpaqueTool(std::vector<std::string> a_fileNames, std::string a_compileOptions);

		OpaqueTool(std::string a_fileName, std::string a_compileOptions);

		virtual
		void
		Setup(std::vector<std::string> a_fileNames, std::string a_compileOptions);

		virtual
		void
		Setup(std::string a_fileName, std::string a_compileOptions);

		virtual
		int
		Run();

		void
		SetReflectionParseOptions(const ReflectionParseOptions& a_options);

		virtual
		int
		GetErrorCode() const;

		virtual
		void
		SetPrintErrorMessage(bool a_shouldPrintMessage);

	private:
		struct Impl;
		Impl* m_impl;
	};
}
