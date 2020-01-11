#pragma once

#include "Oyl3D/Utils/AssetCache.h"

namespace oyl
{
    class Texture
    {
    public:
        enum Format
        {
            RGB8, RGBA8, SRGB8, SRGBA8
        };

        enum Filter
        {
            Nearest, Linear,
        };

        enum Wrap
        {
            Clamp, Mirror, Repeat    
        };
        
    public:
        virtual ~Texture() = default;

        virtual bool load(const std::string& filePath) = 0;
        virtual void unload() = 0;

        virtual void bind(uint slot = 0) const = 0;
        virtual void unbind(uint slot = 0) const = 0;

        virtual bool isLoaded() const = 0;

        const std::string& getFilePath() const { return m_filePath; }

    protected:
        std::string m_filePath;

    private:
        static void init();

        friend class Application;
    };

    class Texture1D : public Texture
    {
        friend Texture;
    public:
        virtual ~Texture1D() = default;

        virtual uint getLength() const = 0;

        static Ref<Texture1D> create(const std::string& filePath);

        static const Ref<Texture1D>& cache(const std::string& filePath,
                                           const CacheAlias& alias = "",
                                           bool overwrite = false);

        static const Ref<Texture1D>& cache(const Ref<Texture1D>& existing,
                                           const CacheAlias&     alias = "",
                                           bool overwrite = false);

        static void discard(const CacheAlias& alias);

        static const Ref<Texture1D>& get(const CacheAlias& alias);

        static bool isCached(const Ref<Texture1D>& existing);

        static bool exists(const CacheAlias& alias);

        static const CacheAlias& getAlias(const Ref<Texture1D>& existing);
        
        static const Ref<Texture1D>& rename(const CacheAlias& currentAlias,
                                            const CacheAlias& newAlias,
                                            bool overwrite = false);

        static const auto& getCache() { return s_cache.m_cache; }

    private:
        static internal::AssetCache<Texture1D> s_cache;
    };

    class Texture2D : public Texture
    {
        friend Texture;
    public:
        virtual ~Texture2D() = default;

        virtual uint getWidth() const = 0;
        virtual uint getHeight() const = 0;

        static Ref<Texture2D> create(const std::string& filePath);

        static const Ref<Texture2D>& cache(const std::string& filePath,
                                           const CacheAlias& alias = "",
                                           bool overwrite = false);

        static const Ref<Texture2D>& cache(const Ref<Texture2D>& existing,
                                           const CacheAlias&     alias = "",
                                           bool overwrite = false);
        
        static void discard(const CacheAlias& alias);

        static const Ref<Texture2D>& get(const CacheAlias& alias);

        static bool isCached(const Ref<Texture2D>& existing);

        static bool exists(const CacheAlias& alias);

        static const CacheAlias& getAlias(const Ref<Texture2D>& existing);

        static const Ref<Texture2D>& rename(const CacheAlias& currentAlias,
                                            const CacheAlias& newAlias,
                                            bool              overwrite = false);

        static const auto& getCache() { return s_cache.m_cache; }

    private:
        static internal::AssetCache<Texture2D> s_cache;
    };

    class Texture3D : public Texture
    {
        friend Texture;
    public:
        virtual ~Texture3D() = default;

        virtual uint getWidth() const = 0;
        virtual uint getHeight() const = 0;
        virtual uint getDepth() const = 0;

        static Ref<Texture3D> create(const std::string& filePath);

        static const Ref<Texture3D>& cache(const std::string& filePath,
                                           const CacheAlias& alias = "",
                                           bool overwrite = false);

        static const Ref<Texture3D>& cache(const Ref<Texture3D>& existing,
                                           const CacheAlias&     alias = "",
                                           bool overwrite = false);

        static void discard(const CacheAlias& alias);

        static const Ref<Texture3D>& get(const CacheAlias& alias);

        static bool isCached(const Ref<Texture3D>& existing);

        static bool exists(const CacheAlias& alias);

        static const CacheAlias& getAlias(const Ref<Texture3D>& existing);

        static const Ref<Texture3D>& rename(const CacheAlias& currentAlias,
                                            const CacheAlias& newAlias,
                                            bool              overwrite = false);

        static const auto& getCache() { return s_cache.m_cache; }

    private:
        static internal::AssetCache<Texture3D> s_cache;
    };

    class TextureCubeMap : public Texture
    {
        friend Texture;
    public:
        virtual ~TextureCubeMap() = default;

        virtual uint getWidth() const = 0;
        virtual uint getHeight() const = 0;

        static Ref<TextureCubeMap> create(const std::string& filePath);

        static const Ref<TextureCubeMap>& cache(const std::string& filePath,
                                                const CacheAlias& alias = "",
                                                bool overwrite = false);

        static const Ref<TextureCubeMap>& cache(const Ref<TextureCubeMap>& existing,
                                                const CacheAlias&     alias = "",
                                                bool overwrite = false);

        static void discard(const CacheAlias& alias);

        static const Ref<TextureCubeMap>& get(const CacheAlias& alias);

        static bool isCached(const Ref<TextureCubeMap>& existing);

        static bool exists(const CacheAlias& alias);

        static const CacheAlias& getAlias(const Ref<TextureCubeMap>& existing);

        static const Ref<TextureCubeMap>& rename(const CacheAlias& currentAlias,
                                                 const CacheAlias& newAlias,
                                                 bool              overwrite = false);

        static const auto& getCache() { return s_cache.m_cache; }

    private:
        static internal::AssetCache<TextureCubeMap> s_cache;
    };
}
