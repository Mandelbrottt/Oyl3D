#pragma once

#include <Tracy/public/tracy/Tracy.hpp>
#if defined(OYL_PROFILE)
#	define OYL_PROFILE_SCOPE(_name_) ZoneScopedN(_name_); OYL_FORCE_SEMICOLON
#	define OYL_PROFILE_SCOPEF(_fmt_, ...) ZoneScoped; ZoneNameF(_fmt_, __VA_ARGS__)
#	define OYL_PROFILE_FUNCTION() ZoneScoped; OYL_FORCE_SEMICOLON
#	define OYL_PROFILE_CUSTOM(...) OYL_MACRO_OVERLOAD(_OYL_PROFILE_CUSTOM, __VA_ARGS__); OYL_FORCE_SEMICOLON

#	define _OYL_PROFILE_CUSTOM_3(_function_, _file_, _line_) _OYL_PROFILE_CUSTOM_4(nullptr, _function_, _file_, _line_)
#	define _OYL_PROFILE_CUSTOM_4(_name_, _function_, _file_, _line_) \
		static const tracy::SourceLocationData OYL_CAT(__tracy_source_location, __LINE__) { \
			_name_, \
			_function_, \
			_file_, \
			_line_, \
			0 \
		}; \
		tracy::ScopedZone _OYL_CAT(___tracy_scoped_zone, __LINE__)(&_OYL_CAT(__tracy_source_location, __LINE__), true);

	// Only the application should have the power to control the profiler
#	if defined(OYL_EXPOSE_PROFILER_CONTROLS)
#		define OYL_PROFILER_INIT()     ::tracy::StartupProfiler()
#		define OYL_PROFILER_SHUTDOWN() ::tracy::ShutdownProfiler()
#		define OYL_PROFILER_WAIT_FOR_CONNECTION()     do {} while (!::tracy::GetProfiler().IsConnected())

#		define OYL_PROFILER_FRAME_MARK()             FrameMark; OYL_FORCE_SEMICOLON
#		define OYL_PROFILER_FRAME_MARK_NAMED(_name_) FrameMarkNamed(_name_); OYL_FORCE_SEMICOLON

#		define OYL_PROFILER_FRAME_MARK_START(_name_) FrameMarkStart(_name_); OYL_FORCE_SEMICOLON
#		define OYL_PROFILER_FRAME_MARK_END(_name_)   FrameMarkEnd(_name_); OYL_FORCE_SEMICOLON
#	endif
#else
#	define OYL_PROFILE_SCOPE(_name_) OYL_FORCE_SEMICOLON
#	define OYL_PROFILE_SCOPEF(_fmt_, ...) OYL_FORCE_SEMICOLON
#	define OYL_PROFILE_FUNCTION()    OYL_FORCE_SEMICOLON
#	define OYL_PROFILE_CUSTOM(...)   OYL_FORCE_SEMICOLON

#	if defined(OYL_EXPOSE_PROFILER_CONTROLS)
#		define OYL_PROFILER_INIT()     OYL_FORCE_SEMICOLON
#		define OYL_PROFILER_SHUTDOWN() OYL_FORCE_SEMICOLON
#		define OYL_PROFILER_WAIT_FOR_CONNECTION()     OYL_FORCE_SEMICOLON

#		define OYL_PROFILER_FRAME_MARK()             OYL_FORCE_SEMICOLON
#		define OYL_PROFILER_FRAME_MARK_NAMED(_name_) OYL_FORCE_SEMICOLON

#		define OYL_PROFILER_FRAME_MARK_START(_name_) OYL_FORCE_SEMICOLON
#		define OYL_PROFILER_FRAME_MARK_END(_name_)   OYL_FORCE_SEMICOLON
#	endif
#endif
