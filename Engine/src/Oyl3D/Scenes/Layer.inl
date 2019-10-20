#pragma once

namespace oyl
{
    template<class T>
    void Layer::scheduleSystemUpdate(Priority priority)
    {
        static bool isInitialized = false;
        OYL_ASSERT(!isInitialized, "Systems should only be initialized once!");

        Ref<ECS::System> newSystem = Ref<T>::create();

        newSystem->setRegistry(registry);

        newSystem->setPostEventCallback(m_postEventCallback);
        m_registerCallback(newSystem, priority);

        newSystem->onEnter();

        m_systems.emplace_back(std::move(newSystem));

        isInitialized = true;
    }
}
