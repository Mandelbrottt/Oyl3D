#pragma once

namespace oyl
{
    enum OylEnum : u32
    {
        None = 0,

        // TODO: Move into Collidable.h 
        Collider_StaticPlane = 1,
        Collider_Box,
        Collider_Sphere,
        Collider_Capsule,
        Collider_Cylinder,
        Collider_Mesh
    };
}
