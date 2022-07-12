#include "PhysicsCapsuleComponent.h"
#include "core/ECS/Entity.h"
#include "physic/PhysicsManager.h"

namespace Physics
{
    PhysicsStaticCapsuleComponentParams::PhysicsStaticCapsuleComponentParams() :
        radius(0.5f),
        halfHeight(1.f),
        staticFriction(0.5f),
        dynamicFriction(0.5f),
        restitution(0.6f),
        isTrigger(false)
    {
    }

    PhysicsDynamicCapsuleComponentParams::PhysicsDynamicCapsuleComponentParams() :
        radius(0.5f),
        halfHeight(1.f),
        staticFriction(0.5f),
        dynamicFriction(0.5f),
        restitution(0.6f),
        density(10.f),
        angularDamping(0.5f),
        isTrigger(false)
    {
    }

    PhysicsDynamicCapsuleComponent::PhysicsDynamicCapsuleComponent(PhysicsDynamicCapsuleComponent&& other) noexcept :
        Component<PhysicsDynamicCapsuleComponent>(other),
        PhysicsRigidDynamic(std::move(other)),
        radius(other.radius),
        halfHeight(other.halfHeight),
        staticFriction(other.staticFriction),
        dynamicFriction(other.dynamicFriction),
        restitution(other.restitution),
        velocity(other.velocity),
        density(other.density),
        angularDamping(other.angularDamping),
        isTrigger(isTrigger)
    {
    }

    void PhysicsDynamicCapsuleComponent::Initialize(const void* params)
    {
        if (params != nullptr)
        {
            const PhysicsDynamicCapsuleComponentParams* initParams = static_cast<const PhysicsDynamicCapsuleComponentParams*>(params);

            radius = initParams->radius;
            halfHeight = initParams->halfHeight;
            staticFriction = initParams->staticFriction;
            dynamicFriction = initParams->dynamicFriction;
            restitution = initParams->restitution;
            velocity = initParams->velocity;
            density = initParams->density;
            angularDamping = initParams->angularDamping;
            isTrigger = initParams->isTrigger;
        }
    }

    void PhysicsDynamicCapsuleComponent::Constructor()
    {
        SetPhysicsEntityHandle(GetEntityHandle());
        CreateCapsuleRigidDynamic(this, Core::Entity::GetEntity(GetEntityHandle()), radius,
            halfHeight, staticFriction, dynamicFriction,
            restitution, velocity, density,
            angularDamping);

        if (isTrigger)
            SetIsTriggerShape(true);
    }

    void PhysicsDynamicCapsuleComponent::Finalize()
    {
        Release();
    }


    PhysicsStaticCapsuleComponent::PhysicsStaticCapsuleComponent(PhysicsStaticCapsuleComponent&& other) noexcept :
        Component<PhysicsStaticCapsuleComponent>(other),
        PhysicsRigidStatic(std::move(other)),
        radius(other.radius),
        halfHeight(other.halfHeight),
        staticFriction(other.staticFriction),
        dynamicFriction(other.dynamicFriction),
        restitution(other.restitution),
        isTrigger(isTrigger)
    {
    }

    void PhysicsStaticCapsuleComponent::Initialize(const void* params)
    {
        if (params != nullptr)
        {
            const PhysicsStaticCapsuleComponentParams* initParams = static_cast<const PhysicsStaticCapsuleComponentParams*>(params);

            radius = initParams->radius;
            halfHeight = initParams->halfHeight;
            staticFriction = initParams->staticFriction;
            dynamicFriction = initParams->dynamicFriction;
            restitution = initParams->restitution;
            isTrigger = initParams->isTrigger;
        }
    }

    void PhysicsStaticCapsuleComponent::Constructor()
    {
        SetPhysicsEntityHandle(GetEntityHandle());
        CreateCapsuleRigidStatic(this, Core::Entity::GetEntity(GetEntityHandle()), radius,
            halfHeight, staticFriction, dynamicFriction,
            restitution);

        if (isTrigger)
            SetIsTriggerShape(true);
    }

    void PhysicsStaticCapsuleComponent::Finalize()
    {
        Release();
    }
}
