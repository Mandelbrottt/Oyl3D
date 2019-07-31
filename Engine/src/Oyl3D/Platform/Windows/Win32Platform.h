#pragma once

#include "Oyl3D/System/Platform.h"

namespace oyl {

class Win32Platform : public Platform {
public:
	Win32Platform();
	virtual float getTimeImpl() override;
};

}