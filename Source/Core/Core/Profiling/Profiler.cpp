#include "pch.h"
#include "Profiler.h"

#include "Core/Time/Time.h"

#include <nlohmann/json.hpp>

namespace Oyl
{
	static
	f64
	GetProfilerTimePoint()
	{
		return Time::Detail::ImmediateElapsedTime() * 1'000'000.0;
	}

	Profiler::Profiler()
		: m_currentSession(nullptr),
		  m_profileCount(0) { }

	void
	Profiler::BeginSession(std::string_view a_name, std::string_view a_filepath)
	{
		// Create directory if it doesn't exist
		auto directory = std::filesystem::path(a_filepath).parent_path();
		create_directories(directory);
		
		m_outputStream.open(a_filepath.data());
		WriteHeader();
		m_currentSession = std::make_unique<ProfilingSession>(ProfilingSession { std::string(a_name) });
	}

	void
	Profiler::EndSession()
	{
		WriteFooter();
		m_currentSession.reset();
		m_outputStream.close();
		m_profileCount = 0;
	}

	void
	Profiler::WriteProfile(const ProfileResult& a_result)
	{
		if (m_profileCount++ > 0)
			m_outputStream << ",";

		std::string name = a_result.name;
		std::replace(name.begin(), name.end(), '"', '\'');

		nlohmann::json profile;
		profile["cat"]  = "function";
		profile["dur"]  = a_result.end - a_result.start;
		profile["name"] = name;
		profile["ph"]   = "X";
		profile["pid"]  = "Oyl3D";
		profile["ts"]   = a_result.start;

		if (auto threadNameIter = m_threadNameMapping.find(a_result.threadId); 
			threadNameIter != m_threadNameMapping.end())
		{
			profile["tid"] = threadNameIter->second;
		} else
		{
			profile["tid"] = a_result.threadId;
		}

		m_outputStream << profile;

		m_outputStream.flush();
	}

	void
	Profiler::WriteHeader()
	{
		m_outputStream << "{\"otherData\": {},\"traceEvents\":[";
		m_outputStream.flush();
	}

	void
	Profiler::WriteFooter()
	{
		m_outputStream << "]}";
		m_outputStream.flush();
	}

	void
	Profiler::RegisterThreadName(std::thread::id a_id, std::string_view a_name)
	{
		auto hash                 = static_cast<u32>(std::hash<std::thread::id>()(a_id));
		m_threadNameMapping[hash] = std::string { a_name };
	}
	
	void
	Profiler::RegisterThreadName(std::string_view a_name)
	{
		RegisterThreadName(std::this_thread::get_id(), a_name);
	}

	Profiler&
	Profiler::Get()
	{
		static Profiler instance;
		return instance;
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

		u32 threadID = static_cast<u32>(std::hash<std::thread::id>()(std::this_thread::get_id()));
		Profiler::Get().WriteProfile({ m_name, m_startTimePoint, endTimePoint, threadID });

		m_stopped = true;
	}
}
