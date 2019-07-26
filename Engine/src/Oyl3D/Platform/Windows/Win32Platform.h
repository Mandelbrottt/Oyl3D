#pragma once

#include "Oyl3D/System/Platform.h"

namespace oyl {

class Win32Platform : public Platform {
public:
	virtual float getTimeImpl() override;
};

}