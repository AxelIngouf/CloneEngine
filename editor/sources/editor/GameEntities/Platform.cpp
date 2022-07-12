#include "Platform.h"

#include "physic/PhysicsShapeComponent/PhysicsBoxComponent.h"
#include "physic/PhysicsShapeComponent/PhysicsMeshComponent.h"
#include "render/RenderComponent/ModelComponent.h"

const Core::Entity* CreatePlatformEntity(Core::SceneNode* entityAnchor)
{
    const Core::Entity* platform = Core::Entity::CreateEntity(entityAnchor);

    {
        // model
        Render::ModelComponentInitParams initParams;
        initParams.file.path = "assets/cube.obj";
        platform->AddComponent<Render::ModelComponent>(&initParams);
        entityAnchor->SetScale({ PLATFORM_XZ_SIZE, 1, PLATFORM_XZ_SIZE });
    }

    {
        // physics
        //Physics::DynamicConvexMeshParams initParams;
        //initParams.file.path = "assets/maps/modular_roads/BigPlateRoadForward.obj";
        //auto* physicsComponent = platform->AddComponent<Physics::PhysicsDynamicConvexMeshComponent>(&initParams);
        //physicsComponent->SetKinematic(true);

        Physics::PhysicsStaticBoxComponentParams initParams;
        initParams.halfExtents = { PLATFORM_XZ_SIZE / 2, 1, PLATFORM_XZ_SIZE / 2 };
        platform->AddComponent<Physics::PhysicsStaticBoxComponent>(&initParams);
    }

    return platform;
}
