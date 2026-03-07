#pragma once

#if OYL_PROFILE
#	include "Profiler_Tracy.h"

#	define OYL_PROFILER_INIT()     ::tracy::StartupProfiler()
#	define OYL_PROFILER_SHUTDOWN() ::tracy::ShutdownProfiler()

#	define OYL_PROFILE_SCOPE(...)       OYL_MACRO_OVERLOAD(_OYL_PROFILE_SCOPE, __VA_ARGS__)
#	define _OYL_PROFILE_SCOPE_1(_name_) ZoneScopedN(_name_)

#	define OYL_PROFILE_FUNCTION() ZoneScoped

#	define OYL_PROFILE_CUSTOM(...) OYL_MACRO_OVERLOAD(_OYL_PROFILE_CUSTOM, __VA_ARGS__)

#	define _OYL_PROFILE_CUSTOM_3(_function_, _file_, _line_) _OYL_PROFILE_CUSTOM_4(nullptr, _function_, _file_, _line_)
#	define _OYL_PROFILE_CUSTOM_4(_name_, _function_, _file_, _line_) \
		static const tracy::SourceLocationData _OYL_CAT(__tracy_source_location, __LINE__) { \
			_name_, \
			_function_, \
			_file_, \
			_line_, \
			0 \
		}; \
		tracy::ScopedZone _OYL_CAT(___tracy_scoped_zone, __LINE__)(&_OYL_CAT(__tracy_source_location, __LINE__), true);

#	define OYL_FRAME_MARK()             FrameMark
#	define OYL_FRAME_MARK_NAMED(_name_) FrameMarkNamed(_name_)

#	define OYL_FRAME_MARK_START(_name_) FrameMarkStart(_name_)
#	define OYL_FRAME_MARK_END(_name_)   FrameMarkEnd(_name_)
#else
#	define OYL_PROFILER_INIT()
#	define OYL_PROFILER_SHUTDOWN()
#	define OYL_PROFILE_SCOPE(...) OYL_UNUSED(__VA_ARGS__)
#	define OYL_PROFILE_FUNCTION()
#	define OYL_PROFILE_CUSTOM(...)
#	define OYL_FRAME_MARK()
#	define OYL_FRAME_MARK_NAMED(_name_) OYL_UNUSED(_name_)
#	define OYL_FRAME_MARK_START(_name_) OYL_UNUSED(_name_)
#	define OYL_FRAME_MARK_END(_name_)   OYL_UNUSED(_name_)
#endif
