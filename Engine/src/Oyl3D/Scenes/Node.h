#pragma once

namespace oyl
{
    class Node
    {
    protected:
        explicit Node() = default;
        explicit Node(std::string name);
        virtual ~Node() = default;

        virtual void onEnter() = 0;
        virtual void onExit() = 0;

        virtual void onUpdate() = 0;
        virtual void onGuiRender() = 0;

        const std::string m_name;
    };
}
