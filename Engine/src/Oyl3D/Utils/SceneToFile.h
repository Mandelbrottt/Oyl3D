#pragma once

namespace oyl
{
    class Material;
    
    namespace internal
    {
        //extern void saveSceneToFile(const Scene& scene);
        //extern void loadSceneFromFile(Scene& scene);
        //extern void saveSceneBackupToFile(const Scene& scene);
        
        extern void registryToSceneFile(entt::registry& registry, const std::string& sceneName);
        extern void registryFromSceneFile(entt::registry& registry, const std::string& sceneName);

        extern entt::entity entityToPrefabFile(entt::registry& registry, const std::string& filepath);
        extern entt::entity entityfromPrefabFile(entt::registry& registry, const std::string& filepath);

        extern Ref<Material> materialFromFile(const std::string& filepath);
        extern void materialToFile(const Ref<Material>& material, const std::string& filepath);
    }
}
