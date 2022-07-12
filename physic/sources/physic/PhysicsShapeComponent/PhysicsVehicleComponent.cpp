#include "PhysicsVehicleComponent.h"

#include "../../../../editor/sources/editor/GameComponents/CarChassisComponent.h"
#include "../../../../editor/sources/editor/GameComponents/CarWheelComponent.h"
#include "core/ECS/Entity.h"
#include "core/scenegraph/SceneNode.h"
#include "physic/PhysicsManager.h"
#include "physic/PhysicsVehicle.h"

namespace Physics
{
    PhysicsVehicleComponentParams::PhysicsVehicleComponentParams() :
        chassisDimensions(defaultChassisDimensions),
        chassisMass(1000.f),
        wheelsRadius(0.4f),
        wheelsMass(25.f),
        wheelsWidth(.3f),
        chassisStaticFriction(0.5f),
        chassisDynamicFriction(0.5f),
        chassisRestitution(0.6f),
        wheelsStaticFriction(0.8f),
        wheelsDynamicFriction(0.8f),
        wheelsRestitution(0.6f)
    {
    }

    PhysicsVehicleComponent::PhysicsVehicleComponent(PhysicsVehicleComponent&& other) noexcept :
        Component<PhysicsVehicleComponent>(other),
        PhysicsVehicleActor(std::move(other)),
        location(other.location),
        rotation(other.rotation),
        wheelsMass(other.wheelsMass),
        wheelsRadius(other.wheelsRadius),
        wheelsWidth(other.wheelsWidth),
        chassisMass(other.chassisMass),
        chassisDimensions(other.chassisDimensions),
        chassisStaticFriction(other.chassisStaticFriction),
        chassisDynamicFriction(other.chassisDynamicFriction),
        chassisRestitution(other.chassisRestitution),
        wheelsStaticFriction(other.wheelsStaticFriction),
        wheelsDynamicFriction(other.wheelsDynamicFriction),
        wheelsRestitution(other.wheelsRestitution)
    {

    }

    void PhysicsVehicleComponent::Initialize(const void* params)
    {
        if (params)
        {
            const PhysicsVehicleComponentParams* initParams = static_cast<const PhysicsVehicleComponentParams*>(params);

            location = initParams->location;
            rotation = initParams->rotation;
            wheelsMass = initParams->wheelsMass;
            wheelsRadius = initParams->wheelsRadius;
            wheelsWidth = initParams->wheelsWidth;
            chassisMass = initParams->chassisMass;
            chassisDimensions = initParams->chassisDimensions;
            chassisStaticFriction = initParams->chassisStaticFriction;
            chassisDynamicFriction = initParams->chassisDynamicFriction;
            chassisRestitution = initParams->chassisRestitution;
            wheelsStaticFriction = initParams->wheelsStaticFriction;
            wheelsDynamicFriction = initParams->wheelsDynamicFriction;
            wheelsRestitution = initParams->wheelsRestitution;
        }
    }

    void PhysicsVehicleComponent::Constructor()
    {
        SetPhysicsEntityHandle(GetEntityHandle());
        CreateVehicle(this, location, rotation, wheelsMass, wheelsRadius,
                      wheelsWidth, chassisMass, chassisDimensions,
                      chassisStaticFriction, chassisDynamicFriction,
                      chassisRestitution, wheelsStaticFriction, wheelsDynamicFriction,
                      wheelsRestitution);
    }

    void PhysicsVehicleComponent::Finalize()
    {
        PhysicsVehicle::ReleaseVehicle(GetVehicleId());
        Release();
    }

    void PhysicsVehicleComponent::UpdatePhysicsRender()
    {
        PhysicsVehicleActor::UpdatePhysicsRender();

        std::vector<Core::Transform> wheelsLocalTransforms;
        GetWheelsLocalTransforms(wheelsLocalTransforms);

        const auto nodeChildren = Core::Entity::GetEntity(handle)->GetAnchor()->GetChildren();

        uint32_t wheelsEntityFound = 0;
        bool isChassisFound = false;

        for(size_t i = 0; i < nodeChildren.Size(); i++)
        {
            if(!isChassisFound && nodeChildren[i]->GetEntity()->GetComponents<CarChassisComponent>().Size() > 0)
            {
                auto chassisTransform = GetChassisTransform();
                chassisTransform.position.y -= 0.8f;
                nodeChildren[i]->SetPosition(chassisTransform.position);
                nodeChildren[i]->SetRotation(chassisTransform.rotation);
                isChassisFound = true;
            }
            else if (wheelsEntityFound < wheelsCount && nodeChildren[i]->GetEntity()->GetComponents<CarWheelComponent>().Size() > 0)
            {
                nodeChildren[i]->SetPosition(wheelsLocalTransforms[wheelsEntityFound].position);
                nodeChildren[i]->SetRotation(wheelsLocalTransforms[wheelsEntityFound].rotation);
                wheelsEntityFound++;
            }
        }
    }
}
