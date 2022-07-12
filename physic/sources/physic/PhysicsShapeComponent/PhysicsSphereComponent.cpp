#include "PhysicsSphereComponent.h"
#include "core/ECS/Entity.h"
#include "physic/PhysicsManager.h"

namespace Physics
{
    PhysicsDynamicSphereComponentParams::PhysicsDynamicSphereComponentParams() :
        radius(0.5f),
        staticFriction(0.5f),
        dynamicFriction(0.5f),
        restitution(0.6f),
        density(10.f),
        angularDamping(0.5f),
        isTrigger(false)
    {
    }

    PhysicsStaticSphereComponentParams::PhysicsStaticSphereComponentParams() :
        radius(0.5f),
        staticFriction(0.5f),
        dynamicFriction(0.5f),
        restitution(0.6f),
        isTrigger(false)
    {
    }

    PhysicsDynamicSphereComponent::PhysicsDynamicSphereComponent(PhysicsDynamicSphereComponent&& other) noexcept :
        Component<PhysicsDynamicSphereComponent>(other),
        PhysicsRigidDynamic(std::move(other)),
        radius(other.radius),
        staticFriction(other.staticFriction),
        dynamicFriction(other.dynamicFriction),
        restitution(other.restitution),
        velocity(other.velocity),
        density(other.density),
        angularDamping(other.angularDamping),
        isTrigger(other.isTrigger)
    {
    }

    void PhysicsDynamicSphereComponent::Initialize(const void* params)
    {
        if (params)
        {
            const PhysicsDynamicSphereComponentParams* initParams = static_cast<const PhysicsDynamicSphereComponentParams*>(params);

            radius = initParams->radius;
            staticFriction = initParams->staticFriction;
            dynamicFriction = initParams->dynamicFriction;
            restitution = initParams->restitution;
            velocity = initParams->velocity;
            density = initParams->density;
            angularDamping = initParams->angularDamping;
            isTrigger = initParams->isTrigger;
        }
    }

    void PhysicsDynamicSphereComponent::Constructor()
    {
        SetPhysicsEntityHandle(GetEntityHandle());

        CreateSphereRigidDynamic(this, Core::Entity::GetEntity(GetEntityHandle()), radius,
            staticFriction, dynamicFriction,
            restitution, velocity, density,
            angularDamping);

        if (isTrigger)
            SetIsTriggerShape(true);
    }

    void PhysicsDynamicSphereComponent::Finalize()
    {
        Release();
    }

    PhysicsStaticSphereComponent::PhysicsStaticSphereComponent(PhysicsStaticSphereComponent&& other) noexcept :
        Component<PhysicsStaticSphereComponent>(other),
        PhysicsRigidStatic(std::move(other)),
        radius(other.radius),
        staticFriction(other.staticFriction),
        dynamicFriction(other.dynamicFriction),
        restitution(other.restitution),
        isTrigger(other.isTrigger)
    {
    }

    void PhysicsStaticSphereComponent::Initialize(const void* params)
    {
        if (params)
        {
            const PhysicsStaticSphereComponentParams* initParams = static_cast<const PhysicsStaticSphereComponentParams*>(params);

            radius = initParams->radius;
            staticFriction = initParams->staticFriction;
            dynamicFriction = initParams->dynamicFriction;
            restitution = initParams->restitution;
            isTrigger = initParams->isTrigger;
        }
    }

    void PhysicsStaticSphereComponent::Constructor()
    {
        SetPhysicsEntityHandle(GetEntityHandle());

        CreateSphereRigidStatic(this, Core::Entity::GetEntity(GetEntityHandle()), radius,
            staticFriction, dynamicFriction, restitution);

        if (isTrigger)
            SetIsTriggerShape(true);
    }

    void PhysicsStaticSphereComponent::Finalize()
    {
        Release();
    }
}
