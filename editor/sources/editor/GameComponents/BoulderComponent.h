#pragma once
#include "core/ECS/Component.h"

constexpr float BOULDER_MINY_FALL = -250.f;

namespace Physics {
    class PhysicsRigidActor;
    struct SweepHit;
}

COMPONENT(BoulderComponent,
    FUNCTION(void, Update, float, elapsedTime)
);
