#include "Boulder.h"


#include "editor/GameComponents/BoulderComponent.h"
#include "physic/PhysicsShapeComponent/PhysicsSphereComponent.h"
#include "physic/PhysicsShapeComponent/PhysicsMeshComponent.h"
#include "render/RenderComponent/ModelComponent.h"

const Core::Entity* CreateBoulderEntity(Core::SceneNode* entityAnchor)
{
    const Core::Entity* platform = Core::Entity::CreateEntity(entityAnchor);

    entityAnchor->SetScale({ BOULDER_RADIUS*2 , BOULDER_RADIUS*2, BOULDER_RADIUS*2 });
    entityAnchor->SetName("Boulder");

    {
        // boulder component
        platform->AddComponent<BoulderComponent>();
    }

    {
        // model
        Render::ModelComponentInitParams initParams;
        initParams.file.path = "assets/padoru/padoru.obj";
        platform->AddComponent<Render::ModelComponent>(&initParams);
    }

    {
        // physics
        Physics::PhysicsDynamicSphereComponentParams initParams;
        initParams.radius = BOULDER_RADIUS;
        platform->AddComponent<Physics::PhysicsDynamicSphereComponent>(&initParams);
    }

    return platform;
}
