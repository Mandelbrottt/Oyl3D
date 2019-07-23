#pragma once

namespace oyl {

class Platform {
public:
	static double getTime() { return s_instance->getTimeImpl(); };
private:
	virtual double getTimeImpl() = 0;
private:
	static Platform* s_instance;
};

}