#pragma once

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

	private:
		IApplicationWindow* m_mainWindow;
	};
}