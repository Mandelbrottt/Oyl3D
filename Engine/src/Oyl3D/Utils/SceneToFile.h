#pragma once
#include "App/Application.h"

namespace oyl
{
    class Scene;

    namespace internal
    {
        extern void saveSceneToFile(const Scene& scene);
        extern void loadSceneFromFile(Scene& scene);
        extern void saveSceneBackupToFile(const Scene& scene);
    }
}
