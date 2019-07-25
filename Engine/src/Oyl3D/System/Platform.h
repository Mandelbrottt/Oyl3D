#pragma once

namespace oyl {

class Platform {
public:
	// Returns current time since execution in seconds
	static double getTime() { return s_instance->getTimeImpl(); };
private:
	virtual double getTimeImpl() = 0;
private:
	static Platform* s_instance;
};

}