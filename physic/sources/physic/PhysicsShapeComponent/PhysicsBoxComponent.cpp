#include "PhysicsBoxComponent.h"

#include "PxPhysicsAPI.h"

#include "core/ECS/Entity.h"
#include "physic/PhysicsInstance.h"
#include "physic/PhysicsManager.h"

namespace Physics
{
    PhysicsStaticBoxComponentParams::PhysicsStaticBoxComponentParams() :
        halfExtents(LibMath::Vector3::One),
        staticFriction(0.5f),
        dynamicFriction(0.5f),
        restitution(0.1f),
        isTrigger(false)
    {
    }

    PhysicsDynamicBoxComponentParams::PhysicsDynamicBoxComponentParams() :
        halfExtents(LibMath::Vector3::One),
        staticFriction(0.5f),
        dynamicFriction(0.5f),
        restitution(0.1f),
        density(10.f),
        angularDamping(0.5f),
        isTrigger(false)
    {
    }

    PhysicsDynamicBoxComponent::PhysicsDynamicBoxComponent(PhysicsDynamicBoxComponent&& other) noexcept :
        Component<PhysicsDynamicBoxComponent>(other),
        PhysicsRigidDynamic(std::move(other)),
        halfExtents(other.halfExtents),
        staticFriction(other.staticFriction),
        dynamicFriction(other.dynamicFriction),
        restitution(other.restitution),
        velocity(other.velocity),
        density(other.density),
        angularDamping(other.angularDamping),
        isTrigger(other.isTrigger)
    {
    }

    void PhysicsDynamicBoxComponent::Initialize(const void* params)
    {
        if (params)
        {
            const PhysicsDynamicBoxComponentParams* initParams = static_cast<const PhysicsDynamicBoxComponentParams*>(params);

            halfExtents = initParams->halfExtents;
            staticFriction = initParams->staticFriction;
            dynamicFriction = initParams->dynamicFriction;
            restitution = initParams->restitution;
            velocity = initParams->velocity;
            density = initParams->density;
            angularDamping = initParams->angularDamping;
            isTrigger = initParams->isTrigger;
        }
    }

    void PhysicsDynamicBoxComponent::Constructor()
    {
        SetPhysicsEntityHandle(GetEntityHandle());
        CreateBoxRigidDynamic(this, Core::Entity::GetEntity(GetEntityHandle()),
            halfExtents, staticFriction, dynamicFriction,
            restitution, velocity, density,
            angularDamping);

        if (isTrigger)
            SetIsTriggerShape(true);
    }

    void PhysicsDynamicBoxComponent::Finalize()
    {
        Release();
    }

    LibMath::Vector3 PhysicsDynamicBoxComponent::GetBoxExtents()
    {
        physx::PxShape* shapes[1];
        rigidDynamic->getShapes(shapes, 1);

        physx::PxBoxGeometry box;
        if(shapes[0]->getBoxGeometry(box))
        {
            return Vec3Convert(box.halfExtents);
        }

        LOG(LOG_WARNING, "Tried to read box geometry extents from a non-box shape.", Core::ELogChannel::CLOG_PHYSICS);

        return {0, 0, 0};
    }

    PhysicsStaticBoxComponent::PhysicsStaticBoxComponent(PhysicsStaticBoxComponent&& other) noexcept :
        Component<PhysicsStaticBoxComponent>(other),
        PhysicsRigidStatic(std::move(other)),
        halfExtents(other.halfExtents),
        staticFriction(other.staticFriction),
        dynamicFriction(other.dynamicFriction),
        restitution(other.restitution),
        isTrigger(other.isTrigger)
    {
    }

    void PhysicsStaticBoxComponent::Initialize(const void* params)
    {
        if (params)
        {
            const PhysicsStaticBoxComponentParams* initParams = static_cast<const PhysicsStaticBoxComponentParams*>(params);

            halfExtents = initParams->halfExtents;
            staticFriction = initParams->staticFriction;
            dynamicFriction = initParams->dynamicFriction;
            restitution = initParams->restitution;
            isTrigger = initParams->isTrigger;
        }
    }

    void PhysicsStaticBoxComponent::Constructor()
    {
        SetPhysicsEntityHandle(GetEntityHandle());
        CreateBoxRigidStatic(this, Core::Entity::GetEntity(GetEntityHandle()), halfExtents,
            staticFriction, dynamicFriction, restitution);

        if (isTrigger)
            SetIsTriggerShape(true);
    }

    void PhysicsStaticBoxComponent::Finalize()
    {
        Release();
    }
}
