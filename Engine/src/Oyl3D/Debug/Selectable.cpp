#include "oylpch.h"
#include "Selectable.h"

namespace oyl::internal
{
    Selectable::Selectable(entt::entity entity, bool isPrefab)
        : m_type(isPrefab ? Type::Prefab : Type::Entity), m_entity(entity) {}
    
    Selectable::Selectable(Ref<Material> material)
        : m_type(Type::Material), m_material(std::move(material)) {}
    
    Selectable::Selectable(Ref<Texture2D> texture2D)
        : m_type(Type::Texture2D), m_texture2D(std::move(texture2D)) {}
}
