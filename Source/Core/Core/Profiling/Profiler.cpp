#include "pch.h"
#include "Profiler.h"

#include <mutex>

#include <nlohmann/json.hpp>

#include "Core/Logging/Logging.h"
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

	static f64 g_timeToFlush = 1.0;

	static std::unique_ptr<ProfilingSession> g_currentSession;
	static std::ofstream                     g_outputStream;
	static i32                               g_profileCount;

	static std::unordered_map<u32, std::string> g_threadNameMapping;

	static std::thread               g_streamWriteThread;
	static std::mutex                g_queueMutex;
	static std::mutex                g_fileMutex;
	static std::queue<ProfileResult> g_profileResultsQueue;
	static std::atomic_bool          g_tryCloseSession;
	static std::condition_variable   g_queuePopulatedCondition;
	static std::condition_variable   g_queueEmptyCondition;

	static bool g_atexitRegistered = false;

	static
	void
	WriteProfile(const ProfileResult& a_result);

	static
	void
	WriteHeader();

	static
	void
	WriteFooter();

	static
	f64
	GetProfilerTimePoint()
	{
		return Time::Detail::ImmediateElapsedTime() * 1'000'000.0;
	}

	static
	void
	WaitForProfileResultsAndWriteToOutputStream()
	{
		while (!g_tryCloseSession)
		{
			ProfileResult result;

			std::unique_lock lock(g_queueMutex);
			g_queuePopulatedCondition.wait(
				lock,
				[]
				{
					return !g_profileResultsQueue.empty() || g_tryCloseSession;
				}
			);
			
			if (!g_profileResultsQueue.empty())
			{
				result = g_profileResultsQueue.front();
				g_profileResultsQueue.pop();
				g_queueEmptyCondition.notify_all();
			}

			lock.unlock();

			if (result.threadId != 0)
			{
				WriteProfile(result);
			}
		}
	}

	static
	void
	CleanupStreamWriteThread()
	{
		std::unique_lock lock(g_queueMutex);
		g_tryCloseSession = true;
		g_queuePopulatedCondition.notify_all();
		if (g_streamWriteThread.joinable())
		{
			g_streamWriteThread.join();
		}
	}

	void
	BeginSession(std::string_view a_name, std::string_view a_filepath)
	{
		if (!g_atexitRegistered)
		{
			// When the engine is force closed, g_streamWriteThread is never cleaned up
			// Do that here
			atexit(CleanupStreamWriteThread);
			g_atexitRegistered = true;
		}

		std::unique_lock lock(g_fileMutex);
		
		OYL_ASSERT(
			!g_outputStream.is_open(),
			"Trying to open profile session \"{}\" while session \"{}\" is active!",
			g_currentSession->name,
			a_name
		);

		// Create directory if it doesn't exist
		auto directory = std::filesystem::path(a_filepath).parent_path();
		create_directories(directory);

		g_outputStream.open(a_filepath.data());
		lock.unlock();
		WriteHeader();
		g_currentSession = std::make_unique<ProfilingSession>(ProfilingSession { std::string(a_name) });

		g_tryCloseSession   = false;
		g_streamWriteThread = std::thread(WaitForProfileResultsAndWriteToOutputStream);
	}

	void
	EndSession()
	{
		OYL_ASSERT(g_outputStream.is_open(), "Trying to close a profile session that doesn't exist!");

		while (true)
		{
			std::unique_lock lock(g_queueMutex);
			if (g_profileResultsQueue.empty())
			{
				g_tryCloseSession = true;
				g_queuePopulatedCondition.notify_all();
				break;
			}
		}

		WriteFooter();
		g_currentSession.reset();
		{
			std::unique_lock lock(g_fileMutex);
			g_outputStream.close();
		}
		g_profileCount = 0;

		if (g_streamWriteThread.joinable())
		{
			g_streamWriteThread.join();
		}
	}

	void
	WriteProfile(const ProfileResult& a_result)
	{
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
		
		std::unique_lock lock(g_fileMutex);
		if (g_profileCount++ > 0)
		{
			g_outputStream << ",";
		}
		g_outputStream << profile;

		// Flush output on an interval as a tradeoff for speed and not losing data
		static auto lastTime = Time::Detail::ImmediateElapsedTime();
		auto currentTime = Time::Detail::ImmediateElapsedTime();
		auto timeSinceLastFlush = currentTime - lastTime;
		if (timeSinceLastFlush > g_timeToFlush)
		{
			g_outputStream.flush();
			lastTime = currentTime;
		}
	}

	void
	WriteHeader()
	{
		std::unique_lock lock(g_fileMutex);
		g_outputStream << "{\"otherData\": {},\"traceEvents\":[";
		g_outputStream.flush();
	}

	void
	WriteFooter()
	{
		std::unique_lock lock(g_fileMutex);
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

		std::unique_lock lock(g_queueMutex);
		g_profileResultsQueue.push(ProfileResult { m_name, m_startTimePoint, endTimePoint, threadId });
		g_queuePopulatedCondition.notify_all();

		m_stopped = true;
	}
}
