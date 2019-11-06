#pragma once

namespace oyl
{
    // TODO: Remove, replace with static Time class
    class Timestep
    {
    public:
        Timestep(float time = 0.0f)
            : m_time(time)
        {
        }

        float getMillis() const { return m_time * 1000.0f; }
        float getSeconds() const { return m_time; }

        // Returns the timestep in seconds
        operator float() const { return m_time; }
    private:
        float m_time;
    };
}
