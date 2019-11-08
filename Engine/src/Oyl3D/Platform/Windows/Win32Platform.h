#pragma once

#include "Oyl3D/Platform//Platform.h"

namespace oyl
{
    class Win32Platform : public Platform
    {
    public:
        Win32Platform();
        virtual float getTimeImpl() override;
    };
}
