#pragma once
#include "core/ECS/Component.h"
#include "../PhysicsRigidActor.h"

namespace Physics
{

    STRUCT(PhysicsDynamicSphereComponentParams,
        FIELD(float, radius),
        FIELD(float, staticFriction),
        FIELD(float, dynamicFriction),
        FIELD(float, restitution),
        FIELD(float, density),
        FIELD(float, angularDamping),
        FIELD(LibMath::Vector3, velocity),
        FIELD(bool, isTrigger),
        SUPPLEMENT(
            EMPTY(),
            PhysicsDynamicSphereComponentParams(); ,
            EMPTY()
        )
    );

    STRUCT(PhysicsStaticSphereComponentParams,
        FIELD(float, radius),
        FIELD(float, staticFriction),
        FIELD(float, dynamicFriction),
        FIELD(float, restitution),
        FIELD(bool, isTrigger),
        SUPPLEMENT(
            EMPTY(),
            PhysicsStaticSphereComponentParams(); ,
            EMPTY()
        )
    );

    COMPONENT(PhysicsDynamicSphereComponent,
        OTHER_BASE(PhysicsRigidDynamic),
        INIT_PARAM(PhysicsDynamicSphereComponentParams),
        FUNCTION(void, Initialize, const void*, params),
        FUNCTION(void, Constructor),
        FUNCTION(void, Finalize),
        FIELD(float, radius),
        FIELD(float, staticFriction),
        FIELD(float, dynamicFriction),
        FIELD(float, restitution),
        FIELD(float, density),
        FIELD(float, angularDamping),
        FIELD(LibMath::Vector3, velocity),
        FIELD(bool, isTrigger),
        SUPPLEMENT(
            EMPTY(),
            PhysicsDynamicSphereComponent() = default;
            PhysicsDynamicSphereComponent(PhysicsDynamicSphereComponent&& other) noexcept;,
            EMPTY()
        )
    );

    COMPONENT(PhysicsStaticSphereComponent,
        OTHER_BASE(PhysicsRigidStatic),
        INIT_PARAM(PhysicsStaticSphereComponentParams),
        FUNCTION(void, Initialize, const void*, params),
        FUNCTION(void, Constructor),
        FUNCTION(void, Finalize),
        FIELD(float, radius),
        FIELD(float, staticFriction),
        FIELD(float, dynamicFriction),
        FIELD(float, restitution),
        FIELD(bool, isTrigger),
        SUPPLEMENT(
            EMPTY(),
            PhysicsStaticSphereComponent() = default;
            PhysicsStaticSphereComponent(PhysicsStaticSphereComponent&& other) noexcept; ,
            EMPTY()
        )
    );
}
