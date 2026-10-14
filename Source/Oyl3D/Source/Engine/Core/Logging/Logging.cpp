#include "Logging.h"

namespace Oyl
{
	ILogger*
	Internal::g_currentLogger = nullptr;

	ILogger*
	Internal::GetCurrentLogger()
	{
		return g_currentLogger;
	}
}
