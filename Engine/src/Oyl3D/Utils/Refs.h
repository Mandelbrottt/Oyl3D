#pragma once

#include <memory>

namespace oyl
{
    template<class T>
    class Ref : public std::shared_ptr<T>
    {
    public:
        using std::shared_ptr<T>::shared_ptr;

        template<class ...TArgs>
        static std::shared_ptr<T> create(TArgs&&... args)
        {
            return std::make_shared<T>(std::forward<TArgs>(args)...);
        }
    };
    
    template<class T>
    class UniqueRef : public std::unique_ptr<T>
    {
    public:
        using std::unique_ptr<T>::unique_ptr;

        template<class ...TArgs>
        static std::unique_ptr<T> create(TArgs&&... args)
        {
            return std::make_unique<T>(std::forward<TArgs>(args)...);
        }
    };
}
