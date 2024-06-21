#pragma once

#include <string>
#include <string_view>
#include <thread>

#include "Core/Typedefs.h"

namespace Oyl
{
	namespace Profiling
	{
		void
		OYL_CORE_API
		BeginSession(std::string_view a_name, std::string_view a_filepath);

		void
		OYL_CORE_API
		EndSession();

		void
		OYL_CORE_API
		RegisterThreadName(std::string_view a_name);

		void
		OYL_CORE_API
		RegisterThreadName(std::thread::id a_id, std::string_view a_name);

		class OYL_CORE_API ProfilingTimer
		{
		public:
			explicit
			ProfilingTimer(const char* a_name);

			~ProfilingTimer();

			void Stop();

		private:
			const char* m_name;
			f64         m_startTimePoint;
			bool        m_stopped;
		};
	};
}

#define OYL_PROFILER_ENABLED 1

#if OYL_PROFILER_ENABLED
#	define OYL_PROFILE_BEGIN_SESSION(_name_, _filepath_) ::Oyl::Profiling::BeginSession(_name_, _filepath_)
#	define OYL_PROFILE_END_SESSION() ::Oyl::Profiling::EndSession()

#	define OYL_PROFILE_SCOPE(_name_, ...) OYL_MACRO_OVERLOAD(_OYL_PROFILE_SCOPE, _name_, __VA_ARGS__)
#	define _OYL_PROFILE_SCOPE_1(_name_) ::Oyl::Profiling::ProfilingTimer _OYL_CAT_EXPAND(timer, __LINE__)(_name_)

#	define OYL_PROFILE_FUNCTION(...) _OYL_EXPAND(OYL_PROFILE_SCOPE(__FUNCSIG__, __VA_ARGS__))
#else
#	define OYL_PROFILE_BEGIN_SESSION(_name_, _filepath_)
#	define OYL_PROFILE_END_SESSION()

#	define OYL_PROFILE_SCOPE(_name_, ...)
#	define OYL_PROFILE_FUNCTION(...)
#endif
