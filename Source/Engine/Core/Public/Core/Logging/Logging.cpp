#include "Logging.h"

#include "Core/Containers/Array.h"

namespace Oyl
{
	class NullLogger : public ILogger
	{
	public:
		void
		Log(LogLevel a_level, StringView a_message, const Debug::SourceLocation& a_location) override
		{
			OYL_UNUSED(a_level, a_message, a_location);
		}

		void
		Flush() override {}

		void
		Transfer(ILogger* a_logger) override
		{
			OYL_UNUSED(a_logger);
		}
	};

	class PreLoadLogger : public ILogger
	{
	public:
		void
		Log(
			LogLevel a_level,
			StringView a_message,
			const Debug::SourceLocation& a_location
		) override
		{
			m_logs.Add({ a_level, String(a_message), a_location });
		}

		void
		Flush() override {}

		void
		Transfer(ILogger* a_logger) override
		{
			for (const auto& [level, message, sourceLocation] : m_logs)
			{
				a_logger->Log(level, message, sourceLocation);
			}
		}

	private:
		struct LogPacket
		{
			LogLevel level;
			String message;
			Debug::SourceLocation sourceLocation;
		};

		Array<LogPacket> m_logs;
	};

	PreLoadLogger g_preLoadLogger;

	static
	ILogger* g_currentLogger = &g_preLoadLogger;

	ILogger*
	Internal::GetCurrentLogger()
	{
		return g_currentLogger;
	}

	void
	Internal::SetCurrentLogger(ILogger* a_logger)
	{
		// If the incoming logger is valid, transfer the old logger's logs to the new logger
		// Otherwise, set logger to empty implementation
		if (a_logger)
		{
			g_currentLogger->Transfer(a_logger);
			g_currentLogger = a_logger;
		} else
		{
			static NullLogger nullLogger;
			g_currentLogger = &nullLogger;
		}
	}
}
