#pragma once

#include "Lights.h"

namespace oyl::component
{
    inline LightSource::LightInfo::LightInfo()
    {
        defaultInit();
    }

    inline LightSource::LightInfo::LightInfo(LightType type)
        : type(type)
    {
        defaultInit();
    }

    inline LightSource::LightInfo::LightInfo(const LightInfo& info)
    {
        *this = info;
    }

    inline LightSource::LightInfo& LightSource::LightInfo::operator=(const LightInfo& info)
    {
        type = info.type;
        switch (info.type)
        {
            case LightType::Point:
                point = info.point;
                break;
            case LightType::Directional:
                directional = info.directional;
                break;
            case LightType::Spot:
                spot = info.spot;
                break;
        }

        return *this;
    }

    inline void LightSource::LightInfo::defaultInit()
    {
        switch (type)
        {
            case LightType::Point:
                new(&point) ::oyl::PointLight();
                break;
            case LightType::Directional:
                new(&directional) ::oyl::DirectionalLight();
                break;
            case LightType::Spot:
                new(&spot) ::oyl::SpotLight();
                break;
        }
    }

    inline LightSource::LightInfo& LightSource::pushLight(LightInfo info)
    {
        return m_lights.emplace_back(std::move(info));
    }

    inline LightSource::LightInfo& LightSource::pushLight(LightType type)
    {
        return m_lights.emplace_back(type);
    }

    inline void LightSource::eraseShape(u32 index)
    {
        m_lights.erase(m_lights.begin() + index);
    }

    inline LightSource::LightInfo& LightSource::getLight(u32 index)
    {
        return m_lights.at(index);
    }

    inline const LightSource::LightInfo& LightSource::getLight(u32 index) const
    {
        return m_lights.at(index);
    }

    inline std::vector<LightSource::LightInfo>& LightSource::getLights()
    {
        return m_lights;
    }

    inline const std::vector<LightSource::LightInfo>& LightSource::getLights() const
    {
        return m_lights;
    }

    inline std::size_t LightSource::size() const
    {
        return m_lights.size();
    }

    inline bool LightSource::empty() const
    {
        return m_lights.empty();
    }

    inline std::vector<LightSource::LightInfo>::iterator LightSource::begin()
    {
        return m_lights.begin();
    }

    inline std::vector<LightSource::LightInfo>::iterator LightSource::end()
    {
        return m_lights.end();
    }

    inline std::vector<LightSource::LightInfo>::const_iterator LightSource::begin() const
    {
        return m_lights.cbegin();
    }

    inline std::vector<LightSource::LightInfo>::const_iterator LightSource::end() const
    {
        return m_lights.cend();
    }
}
