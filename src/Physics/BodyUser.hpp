#pragma once
#include <functional>

namespace projectiles{
    class Projectile;
}

class BodyUser
{
public:
    std::function<bool(projectiles::Projectile&)> processHit = [](projectiles::Projectile&){ return false; };
    std::function<void()> processCollision = [](){ }; // nullfunction or if? benchmark it.
};
