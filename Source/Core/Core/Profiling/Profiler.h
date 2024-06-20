#pragma once

#include <fstream>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>

#include "Core/Typedefs.h"

namespace Oyl
{
	struct ProfileResult
	{
		std::string name;
		f64         start;
		f64         end;
		u32         threadId;
	};

	struct ProfilingSession
	{
		std::string name;
	};

	class OYL_CORE_API Profiler
	{
	public:
		Profiler();

		void
		BeginSession(std::string_view a_name, std::string_view a_filepath);

		void
		EndSession();

		void
		WriteProfile(const ProfileResult& a_result);

		void
		WriteHeader();

		void
		WriteFooter();

		void
		RegisterThreadName(std::string_view a_name);

		void
		RegisterThreadName(std::thread::id a_id, std::string_view a_name);

		static
		Profiler&
		Get();

	private:
		std::unique_ptr<ProfilingSession>    m_currentSession;
		std::ofstream                        m_outputStream;
		i32                                  m_profileCount;
		std::unordered_map<u32, std::string> m_threadNameMapping;
	};

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
}
