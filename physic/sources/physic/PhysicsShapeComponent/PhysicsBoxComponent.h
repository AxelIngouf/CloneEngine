#pragma once
#include "core/ECS/Component.h"
#include "../PhysicsRigidActor.h"

namespace Physics
{
    STRUCT(PhysicsStaticBoxComponentParams,
        FIELD(LibMath::Vector3, halfExtents),
        FIELD(float, staticFriction),
        FIELD(float, dynamicFriction),
        FIELD(float, restitution),
        FIELD(bool, isTrigger),
        SUPPLEMENT(
            EMPTY(),
            PhysicsStaticBoxComponentParams(); ,
            EMPTY()
        )
    );

    STRUCT(PhysicsDynamicBoxComponentParams,
        FIELD(LibMath::Vector3, halfExtents),
        FIELD(float, staticFriction),
        FIELD(float, dynamicFriction),
        FIELD(float, restitution),
        FIELD(float, density),
        FIELD(float, angularDamping),
        FIELD(LibMath::Vector3, velocity),
        FIELD(bool, isTrigger),
        SUPPLEMENT(
            EMPTY(),
            PhysicsDynamicBoxComponentParams();,
            EMPTY()
        )
    );

    COMPONENT(PhysicsDynamicBoxComponent,
        INIT_PARAM(PhysicsDynamicBoxComponentParams),
        OTHER_BASE(PhysicsRigidDynamic),
        FUNCTION(void, Initialize, const void*, params),
        FUNCTION(void, Constructor),
        FUNCTION(void, Finalize),
        FUNCTION(LibMath::Vector3, GetBoxExtents),
        FIELD(LibMath::Vector3, halfExtents),
        FIELD(float, staticFriction),
        FIELD(float, dynamicFriction),
        FIELD(float, restitution),
        FIELD(float, density),
        FIELD(float, angularDamping),
        FIELD(LibMath::Vector3, velocity),
        FIELD(bool, isTrigger),
        SUPPLEMENT(
            EMPTY(),
            PhysicsDynamicBoxComponent() = default;
            PhysicsDynamicBoxComponent(PhysicsDynamicBoxComponent&& other) noexcept;,
            EMPTY()
        )
    );

    COMPONENT(PhysicsStaticBoxComponent,
        INIT_PARAM(PhysicsStaticBoxComponentParams),
        OTHER_BASE(PhysicsRigidStatic),
        FUNCTION(void, Initialize, const void*, params),
        FUNCTION(void, Constructor),
        FUNCTION(void, Finalize),
        FIELD(LibMath::Vector3, halfExtents),
        FIELD(float, staticFriction),
        FIELD(float, dynamicFriction),
        FIELD(float, restitution),
        FIELD(bool, isTrigger),
        SUPPLEMENT(
            EMPTY(),
            PhysicsStaticBoxComponent() = default;
            PhysicsStaticBoxComponent(PhysicsStaticBoxComponent&& other) noexcept;,
        EMPTY()
        )
    );
}

