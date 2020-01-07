#pragma once

#include "Animatable.h"

namespace oyl::component
{
    inline void Animator::pushAnimation(const std::string& alias, const Ref<Animation>& animation)
    {
        OYL_ASSERT(m_animations.find(alias) == m_animations.end(), "Animation '{0}' already exists!", alias);

        m_animations[alias] = animation;
    }

    inline void Animator::setNextAnimation(const std::string& alias, f32 transitionDuration)
    {
        auto it = m_animations.find(alias);
        OYL_ASSERT(it != m_animations.end(), "Animation '{0}' was not found!", alias);

        m_nextAnimation      = it->second;
        m_transitionElapsed  = 0.0f;
        m_transitionDuration = transitionDuration;
    }

    inline bool Animator::getBool(const std::string& name) const
    {
        OYL_ASSERT(m_bools.find(name) != m_bools.end(), "Bool '{0}' was not found!", name);

        return m_bools.at(name);
    }

    inline void Animator::setBool(const std::string& name, bool value)
    {
        m_bools[name] = value;
    }

    inline const Ref<VertexArray>& Animator::getVertexArray() const
    {
        return m_vao;
    }
}