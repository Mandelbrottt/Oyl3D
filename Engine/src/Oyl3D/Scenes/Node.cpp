#include "oylpch.h"
#include "Node.h"

namespace oyl
{
    Node::Node(std::string name)
        : m_name(std::move(name))
    {
    }
}