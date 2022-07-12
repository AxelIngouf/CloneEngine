#pragma once
#include "core/ECS/Component.h"
#include "../PhysicsRigidActor.h"

namespace Physics
{

    namespace Wrapper // todo: replace with real reflection data
    {
        inline static const Core::Type PhysicsVehicleActor_("PhysicsVehicleActor", Core::ETypeCategory::WRAPPER, sizeof(PhysicsVehicleActor), 8);
    }

    inline static LibMath::Vector3 defaultChassisDimensions = LibMath::Vector3(2.5f, 1.3f, 5.0f);

    STRUCT(PhysicsVehicleComponentParams,
        FIELD(LibMath::Vector3, location),
        FIELD(LibMath::Quaternion, rotation),
        FIELD(LibMath::Vector3, chassisDimensions),
        FIELD(float, chassisMass),
        FIELD(float, wheelsRadius),
        FIELD(float, wheelsMass),
        FIELD(float, wheelsWidth),
        FIELD(float, chassisStaticFriction),
        FIELD(float, chassisDynamicFriction),
        FIELD(float, chassisRestitution),
        FIELD(float, wheelsStaticFriction),
        FIELD(float, wheelsDynamicFriction),
        FIELD(float, wheelsRestitution),
        SUPPLEMENT(
            EMPTY(),
            PhysicsVehicleComponentParams();,
            EMPTY()
        )
    );

    COMPONENT(PhysicsVehicleComponent,
        OTHER_BASE(PhysicsVehicleActor),
        INIT_PARAM(PhysicsVehicleComponentParams),
        FUNCTION(void, Initialize, const void*, params),
        FUNCTION(void, Constructor),
        FUNCTION(void, Finalize),
        FUNCTION(void, UpdatePhysicsRender),
        FIELD(LibMath::Vector3, location),
        FIELD(LibMath::Quaternion, rotation),
        FIELD(LibMath::Vector3, chassisDimensions),
        FIELD(float, chassisMass),
        FIELD(float, wheelsRadius),
        FIELD(float, wheelsMass),
        FIELD(float, wheelsWidth),
        FIELD(float, chassisStaticFriction),
        FIELD(float, chassisDynamicFriction),
        FIELD(float, chassisRestitution),
        FIELD(float, wheelsStaticFriction),
        FIELD(float, wheelsDynamicFriction),
        FIELD(float, wheelsRestitution),
        SUPPLEMENT(
            EMPTY(),
            PhysicsVehicleComponent() = default;
            PhysicsVehicleComponent(PhysicsVehicleComponent&& other) noexcept;,
            EMPTY()
        )
    );
}

