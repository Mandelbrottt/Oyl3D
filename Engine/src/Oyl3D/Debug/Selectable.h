#pragma once

namespace oyl
{
    class Material;
    class Texture2D;
    
    namespace internal
    {
        class Selectable
        {
        public:
            enum class Type
            {
                None,
                Entity,
                Prefab,
                Material,
                Texture2D
            };

            Selectable() = default;
            Selectable(entt::entity entity, bool isPrefab = false);
            Selectable(Ref<Material> material);
            Selectable(Ref<Texture2D> texture2D);

            Type type() const { return m_type; }
            entt::entity entity() const { return m_entity; }
            const Ref<Material>& material() const { return m_material; }
            const Ref<Texture2D>& texture2D() const { return m_texture2D; }
            
        private:
            Type m_type = Type::None;

            entt::entity m_entity = entt::null;

            Ref<Material> m_material = nullptr;
            Ref<Texture2D> m_texture2D = nullptr;

        };
    }
}