#pragma once
#include "core/ECS/Component.h"
#include "../PhysicsRigidActor.h"

namespace Physics
{
    STRUCT(PhysicsStaticCapsuleComponentParams,
        FIELD(float, radius),
        FIELD(float, halfHeight),
        FIELD(float, staticFriction),
        FIELD(float, dynamicFriction),
        FIELD(float, restitution),
        FIELD(bool, isTrigger),
        SUPPLEMENT(
            EMPTY(),
            PhysicsStaticCapsuleComponentParams(); ,
            EMPTY()
        )
    );

    STRUCT(PhysicsDynamicCapsuleComponentParams,
        FIELD(float, radius),
        FIELD(float, halfHeight),
        FIELD(float, staticFriction),
        FIELD(float, dynamicFriction),
        FIELD(float, restitution),
        FIELD(float, density),
        FIELD(float, angularDamping),
        FIELD(LibMath::Vector3, velocity),
        FIELD(bool, isTrigger),
        SUPPLEMENT(
            EMPTY(),
            PhysicsDynamicCapsuleComponentParams(); ,
            EMPTY()
        )
    );

    COMPONENT(PhysicsDynamicCapsuleComponent,
        OTHER_BASE(PhysicsRigidDynamic),
        INIT_PARAM(PhysicsDynamicCapsuleComponentParams),
        FUNCTION(void, Initialize, const void*, params),
        FUNCTION(void, Constructor),
        FUNCTION(void, Finalize),
        FIELD(float, radius),
        FIELD(float, halfHeight),
        FIELD(float, staticFriction),
        FIELD(float, dynamicFriction),
        FIELD(float, restitution),
        FIELD(float, density),
        FIELD(float, angularDamping),
        FIELD(LibMath::Vector3, velocity),
        FIELD(bool, isTrigger),
        SUPPLEMENT(
            EMPTY(),
            PhysicsDynamicCapsuleComponent() = default;
            PhysicsDynamicCapsuleComponent(PhysicsDynamicCapsuleComponent&& other) noexcept;,
        EMPTY()
        )
    );

    COMPONENT(PhysicsStaticCapsuleComponent,
        OTHER_BASE(PhysicsRigidStatic),
        INIT_PARAM(PhysicsStaticCapsuleComponentParams),
        FUNCTION(void, Initialize, const void*, params),
        FUNCTION(void, Constructor),
        FUNCTION(void, Finalize),
        FIELD(float, radius),
        FIELD(float, halfHeight),
        FIELD(float, staticFriction),
        FIELD(float, dynamicFriction),
        FIELD(float, restitution),
        FIELD(bool, isTrigger),
        SUPPLEMENT(
            EMPTY(),
            PhysicsStaticCapsuleComponent() = default;
            PhysicsStaticCapsuleComponent(PhysicsStaticCapsuleComponent&& other) noexcept; ,
        EMPTY()
        )
    );

}

