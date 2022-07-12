#include "PlatformComponent.h"

#include "core/ECS/Entity.h"
#include "editor/GameEntities/Platform.h"
#include "physic/PhysicsShapeComponent/PhysicsBoxComponent.h"
#include "physic/PhysicsShapeComponent/PhysicsMeshComponent.h"

PlatformComponentParams::PlatformComponentParams() :
    platformDirection(EPlatformDirection::FORWARD),
    fallSecondTimer(20.f)
{
}

void PlatformComponent::Initialize(const void* params)
{
    if(params != nullptr)
    {
        const PlatformComponentParams* initParams = static_cast<const PlatformComponentParams*>(params);

        fallSecondTimer = initParams->fallSecondTimer;
        platformDirection = initParams->platformDirection;
    }
    else
    {
        fallSecondTimer = 20.f;
    }

    totalElapsedTime = 0.f;
    isFalling = false;
    platformId = -1;
}

void PlatformComponent::Update(float elapsedTime)
{
    totalElapsedTime += elapsedTime;
    if(!isFalling && totalElapsedTime >= fallSecondTimer)
    {
        Fall();
    }
    else if(Core::Entity::GetEntity(GetEntityHandle())->GetAnchor()->GetWorldTransformNoCheck().position.y < PLATFORM_MINY_FALL || totalElapsedTime > fallSecondTimer * 3)
    {
        Core::Entity::GetEntity(GetEntityHandle())->GetAnchor()->Destroy();
    }
}

void PlatformComponent::Fall()
{
    isFalling = true;

    auto physicsComponents = Core::Entity::GetEntity(GetEntityHandle())->GetComponents<Physics::PhysicsStaticBoxComponent>();

    if(physicsComponents.Size() > 0)
    {
        Core::Entity::GetEntity(GetEntityHandle())->DestroyComponent(physicsComponents[0]);
        Physics::PhysicsDynamicBoxComponentParams physicsCompParams;
        physicsCompParams.halfExtents = { PLATFORM_XZ_SIZE / 2, 1, PLATFORM_XZ_SIZE / 2 };
        Core::Entity::GetEntity(GetEntityHandle())->AddComponent<Physics::PhysicsDynamicBoxComponent>(&physicsCompParams);
    }
}