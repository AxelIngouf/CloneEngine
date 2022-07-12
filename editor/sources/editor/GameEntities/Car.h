#pragma once
#include "core/ECS/Entity.h"

namespace Render {
    struct ModelComponent;
}

namespace Physics {
    struct PhysicsVehicleComponent;
}

const Core::Entity* CreateCarEntity(Core::SceneNode* entityAnchor);
