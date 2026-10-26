#pragma once

#include <Core/Containers/Array.h>

#include "ApplicationWindow.h"

namespace Oyl
{
	using OnUpdateDelegate = Delegate<void()>;

	class IApplication
	{
		friend class ApplicationModule;

	public:
		virtual
		~IApplication() = 0;

	protected:
		virtual
		void
		Start() = 0;

		virtual
		void
		Update() = 0;

		virtual
		void
		Shutdown() = 0;

		virtual
		void
		WaitIdle() = 0;

	public:
		virtual
		const Array<IApplicationWindow*>&
		GetApplicationWindows() const = 0;

		virtual
		IApplicationWindow*
		CreateApplicationWindow(const ApplicationWindowCreateParams& a_params) = 0;

		virtual
		void
		CloseApplicationWindow(IApplicationWindow* a_window) = 0;

		virtual
		void
		RequestExit(int a_exitCode = 0) = 0;

		virtual
		void
		ForceExit(int a_exitCode = 0) = 0;

		virtual
		bool
		IsExitRequested(int* a_outExitCode = nullptr) = 0;

		virtual
		uint32
		TargetFramerate() = 0;

		virtual
		void
		SetTargetFramerate(uint32 a_target) = 0;

		virtual
		const MulticastDelegate<void()>&
		OnUpdateCallback() const = 0;

		virtual
		const MulticastDelegate<void()>&
		OnDebugGuiCallback() const = 0;
	};

	inline
	IApplication::~IApplication() {}
}
