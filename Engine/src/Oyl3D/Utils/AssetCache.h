#pragma once

namespace oyl
{
    template<class T> class Ref;
}

namespace oyl::internal
{
    template<class T>
    class AssetCache
    {
        friend T;
    public:
        const Ref<T>& cache(const std::string& filePath,
                            CacheAlias alias = "",
                            bool overwrite = false);

        const Ref<T>& cache(const Ref<T>&     existing,
                            const CacheAlias& alias     = "",
                            bool              overwrite = true);

        void discard(const CacheAlias& alias);

        const Ref<T>& get(const CacheAlias& alias);

        const Ref<T>& rename(const CacheAlias& currentAlias,
                             const CacheAlias& newAlias,
                             bool overwrite = false);

    protected:
        std::unordered_map<CacheAlias, Ref<T>> m_cache;

        static const char* s_typename;
    };
}

#include "AssetCache.inl"
