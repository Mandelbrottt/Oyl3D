#pragma once

#include <Core/Events/Event.h>

namespace Oyl
{
	class IApplicationWindow;

	class EditorClientMainLoop
	{
	public:
		EditorClientMainLoop();

		~EditorClientMainLoop();

		void
		Update();

		void
		PostEvent(const Event& a_event) const;

	private:
		IApplicationWindow* m_mainWindow;
	};
}