#pragma once

namespace oyl {

	class Application {
	public:
		Application();
		virtual ~Application();

		void run();
	private:
		bool m_running;
	};

	Application* createApplication();

}
