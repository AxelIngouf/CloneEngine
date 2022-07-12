#include "Checkpoint.h"


#include "editor/GameComponents/CheckpointComponent.h"
#include "render/RenderComponent/ModelComponent.h"
#include "physic/PhysicsShapeComponent/PhysicsBoxComponent.h"
#include "physic/PhysicsShapeComponent/PhysicsMeshComponent.h"
#include "sound/Sound3D.h"

const Core::Entity* CreateCheckpointEntity(Core::SceneNode* entityAnchor)
{
    const Core::Entity* checkpoint = Core::Entity::CreateEntity(entityAnchor);

    Core::EntityHandle checkpointHandle = checkpoint->GetHandle();

    {
        // Model
        Render::ModelComponentInitParams initParams;
        initParams.file.path = "assets/maps/modular/ark_white.obj";
        checkpoint->AddComponent<Render::ModelComponent>(&initParams);
    }

    {
        // Trigger box
        Physics::PhysicsStaticBoxComponentParams initParams;
        initParams.halfExtents = { 3, 5, 0.2f };
        initParams.isTrigger = true;
        checkpoint->AddComponent<Physics::PhysicsStaticBoxComponent>(&initParams);
    }

    {
        // Checkpoint component
        checkpoint->AddComponent<CheckpointComponent>();
    }

    {
        // Sound
        Sound::SoundComponentParams initParams;
        initParams.isLoop = false;
        checkpoint->AddComponent<Sound::Sound3DComponent>(&initParams);
    }

    {
        // Collision mesh
        auto* collisionNode = entityAnchor->CreateChild();
        collisionNode->SetName("Collision");
        Physics::TriangleMeshInitParams initParams;
        initParams.file.path = "assets/maps/modular/ark_white.obj";
        collisionNode->GetEntity()->AddComponent<Physics::PhysicsStaticTriangleMeshComponent>(&initParams);
        
    }

    return Core::Entity::GetEntity(checkpointHandle);
}
