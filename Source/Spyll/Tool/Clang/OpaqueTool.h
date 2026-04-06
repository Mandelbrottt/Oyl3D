#pragma once

#include <string>

#include "Spyll/Tool/Core/ReflectionDescriptor.h"

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

		void
		Setup(std::vector<std::string> a_fileNames, std::string a_compileOptions);

		void
		Setup(std::string a_fileName, std::string a_compileOptions);
		
		int
		Run();

		int
		GetErrorCode() const;

		void
		SetPrintErrorMessage(bool a_shouldPrintMessage);

		ReflectionDescriptor
		GetMergedReflectionDescriptor() const;

	private:
		struct Impl;
		Impl* m_impl;
	};
}
