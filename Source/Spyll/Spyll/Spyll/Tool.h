#pragma once

#include <string>

namespace Spyll
{
	class Tool
	{
	public:
		explicit
		Tool(const std::string& a_fileName, const std::string& a_compileArgs);

		int
		Run();

		int
		GetErrorCode() const { return m_errorCode; }

		const std::string&
		GetDatabaseError() const { return m_databaseError; }

	private:
		std::string m_file;
		std::string m_compileArgs;

		int m_errorCode = 0;
		std::string m_databaseError;
	};
}
