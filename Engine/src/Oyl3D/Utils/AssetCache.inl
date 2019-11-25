#pragma once
#include "AssetCache.h"

namespace oyl::internal
{    
    template<class T>
    const char* AssetCache<T>::s_typename = "<Invalid>";

    template<class T>
    const Ref<T>& AssetCache<T>::cache(const std::string& filePath,
                                       CacheAlias         alias,
                                       bool               overwrite)
    {
        // If no alias is given, make it the extensionless file name
        if (alias.empty())
        {
            auto lastSlash = filePath.find_last_of("/\\");
            lastSlash      = lastSlash == std::string::npos ? 0 : lastSlash + 1;
            auto lastDot   = filePath.rfind('.');
            auto count     = lastDot == std::string::npos ? filePath.size() - lastSlash : lastDot - lastSlash;
            alias          = filePath.substr(lastSlash, count);
        }

        Ref<T> alreadyCached = nullptr;

        // If the mesh is already cached, get a handle to that mesh
        auto it = m_cache.begin();
        for (; it != m_cache.end(); ++it)
        {
            if (it->second->getFilePath() == filePath)
            {
                alreadyCached = it->second;
            }
        }

        if (!overwrite && m_cache.find(alias) != m_cache.end())
        {
            OYL_LOG_WARN("{0} with alias '{1}' already exists!", s_typename, alias);
            return m_cache.at(alias);
        }

        // Return the cached mesh
        m_cache[alias] = alreadyCached != nullptr ? alreadyCached : T::create(filePath);
        return m_cache[alias];
    }

    template<class T>
    const Ref<T>& AssetCache<T>::cache(const Ref<T>&     existing,
                                       const CacheAlias& alias,
                                       bool              overwrite)
    {
        Ref<T> alreadyCached = nullptr;

        for (auto& kvp : m_cache)
        {
            if (kvp.second == existing)
            {
                alreadyCached = kvp.second;
            }
        }

        auto it = m_cache.find(alias);
        if (it != m_cache.end())
        {
            if (!overwrite)
            {
                OYL_LOG_WARN("{0} '{1}' is already cached!", s_typename, alias);
                return it->second;
            }
            else
            {
                it->second = alreadyCached != nullptr ? alreadyCached : existing;
                return it->second;
            }
        }

        m_cache[alias] = existing;
        return existing;
    }

    template<class T>
    void AssetCache<T>::discard(const CacheAlias& alias)
    {
        auto it = m_cache.find(alias);
        if (it == m_cache.end())
        {
            OYL_LOG_ERROR("{0} '{1}' could not be discarded because it does not exist.", s_typename, alias);
        }
        else m_cache.erase(it);
    }

    template<class T>
    const Ref<T>& AssetCache<T>::get(const CacheAlias& alias)
    {
        // If the mesh has not been cached, return the default 'invalid' model
        if (m_cache.find(alias) == m_cache.end())
        {
            OYL_LOG_ERROR("{0} '{1}' not found!", s_typename, alias);
            return this->get(INVALID_ALIAS);
        }
        return m_cache.at(alias);
    }

    template<class T>
    bool AssetCache<T>::isCached(const Ref<T>& existing)
    {
        // Return true if the Ref is already cached, otherwise return false
        for (const auto& kvp : m_cache)
        {
            if (kvp.second == existing)
                return true;
        }
        return false;
    }

    template<class T> bool AssetCache<T>::exists(const CacheAlias& alias)
    {
        return m_cache.find(alias) != m_cache.end();
    }

    template<class T>
    const CacheAlias& AssetCache<T>::getAlias(const Ref<T>& existing)
    {
        // If the Ref is already cached, return the first alias in the list.
        // Otherwise, return the 'invalid' alias
        for (const auto& kvp : m_cache)
        {
            if (kvp.second == existing)
                return kvp.first;
        }
        return INVALID_ALIAS;
    }
    
    template<class T>
    const Ref<T>& AssetCache<T>::rename(const CacheAlias& currentAlias,
                                        const CacheAlias& newAlias,
                                        bool              overwrite)
    {
        // The mesh has to exist to be renamed
        auto currIt = m_cache.find(currentAlias);
        if (currIt == m_cache.end())
        {
            OYL_LOG_ERROR("{0} '{1}' was not found!", s_typename, currentAlias);
            return this->get(INVALID_ALIAS);
        }

        // Special case if a mesh with alias $newAlias already exists
        auto newIt = m_cache.find(newAlias);
        if (newIt != m_cache.end())
        {
            if (newIt == currIt) return currIt->second;

            if (!overwrite)
            {
                OYL_LOG_ERROR("Failed to rename {0} '{1}', alias '{2}' already exists!",
                              s_typename, currentAlias, newAlias);
                return currIt->second;
            }

            // Warn the user if they are overiding a mesh of a different type
            if (currIt->second->getFilePath() != newIt->second->getFilePath())
            {
                OYL_LOG_INFO("{2} '{3}' with info '{0}' was replaced by '{1}'.",
                             currIt->second->getFilePath(),
                             newIt->second->getFilePath(),
                             s_typename, newAlias);
            }
        }

        // Delete the existing alias and create the new one
        m_cache[newAlias] = currIt->second;
        m_cache.erase(currIt);

        return m_cache[newAlias];
    }
}
