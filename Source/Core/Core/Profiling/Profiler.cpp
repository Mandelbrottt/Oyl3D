#include "pch.h"
#include "Profiler.h"


#include <nlohmann/json.hpp>

#include "Core/Time/Time.h"

namespace Oyl::Profiling
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

	static
	void
	WriteProfile(const ProfileResult& a_result);

	static
	void
	WriteHeader();

	static
	void
	WriteFooter();

	static std::unique_ptr<ProfilingSession> g_currentSession;
	static std::ofstream                     g_outputStream;
	static i32                               g_profileCount;

	static std::unordered_map<u32, std::string> g_threadNameMapping;

	static
	f64
	GetProfilerTimePoint()
	{
		return Time::Detail::ImmediateElapsedTime() * 1'000'000.0;
	}

	void
	BeginSession(std::string_view a_name, std::string_view a_filepath)
	{
		// Create directory if it doesn't exist
		auto directory = std::filesystem::path(a_filepath).parent_path();
		create_directories(directory);

		g_outputStream.open(a_filepath.data());
		WriteHeader();
		g_currentSession = std::make_unique<ProfilingSession>(ProfilingSession { std::string(a_name) });
	}

	void
	EndSession()
	{
		WriteFooter();
		g_currentSession.reset();
		g_outputStream.close();
		g_profileCount = 0;
	}

	void
	WriteProfile(const ProfileResult& a_result)
	{
		if (g_profileCount++ > 0)
			g_outputStream << ",";

		std::string name = a_result.name;
		std::replace(name.begin(), name.end(), '"', '\'');

		nlohmann::json profile;
		profile["cat"]  = "function";
		profile["dur"]  = a_result.end - a_result.start;
		profile["name"] = name;
		profile["ph"]   = "X";
		profile["pid"]  = "Oyl3D";
		profile["ts"]   = a_result.start;

		if (auto threadNameIter = g_threadNameMapping.find(a_result.threadId);
			threadNameIter != g_threadNameMapping.end())
		{
			profile["tid"] = threadNameIter->second;
		} else
		{
			profile["tid"] = a_result.threadId;
		}


		g_outputStream << profile;
		g_outputStream.flush();
	}

	void
	WriteHeader()
	{

		g_outputStream << "{\"otherData\": {},\"traceEvents\":[";
		g_outputStream.flush();
	}

	void
	WriteFooter()
	{

		g_outputStream << "]}";
		g_outputStream.flush();
	}

	void
	RegisterThreadName(std::thread::id a_id, std::string_view a_name)
	{
		auto hash                 = static_cast<u32>(std::hash<std::thread::id>()(a_id));
		g_threadNameMapping[hash] = std::string { a_name };
	}

	void
	RegisterThreadName(std::string_view a_name)
	{
		RegisterThreadName(std::this_thread::get_id(), a_name);
	}
	ProfilingTimer::ProfilingTimer(const char* a_name)
		: m_name(a_name),
		  m_stopped(false)
	{
		m_startTimePoint = GetProfilerTimePoint();
	}

	ProfilingTimer::~ProfilingTimer()
	{
		if (!m_stopped)
			Stop();
	}

	void
	ProfilingTimer::Stop()
	{
		auto endTimePoint = GetProfilerTimePoint();

		u32 threadId = static_cast<u32>(std::hash<std::thread::id>()(std::this_thread::get_id()));
		WriteProfile({ m_name, m_startTimePoint, endTimePoint, threadId });
		m_stopped = true;
	}
}
