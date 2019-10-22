#pragma once

namespace oyl
{
    class Node
    {
    protected:
        explicit Node(std::string name);

        std::string m_name;
    };
}
