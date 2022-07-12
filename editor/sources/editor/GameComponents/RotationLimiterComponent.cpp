#include "RotationLimiterComponent.h"

#include "core/ECS/Entity.h"
#include "core/scenegraph/SceneNode.h"

RotationLimiterComponentParams::RotationLimiterComponentParams() :
    pitchMinLimit(-180.f),
    rollMinLimit(-180.f),
    yawMinLimit(-180.f),
    pitchMaxLimit(180.f),
    rollMaxLimit(180.f),
    yawMaxLimit(180.f)
{
}

void RotationLimiterComponent::Initialize(const void* params)
{
    if(params != nullptr)
    {
        const RotationLimiterComponentParams* limiterParams = static_cast<const RotationLimiterComponentParams*>(params);

        pitchMinLimit = limiterParams->pitchMinLimit;
        rollMinLimit = limiterParams->rollMinLimit;
        yawMinLimit = limiterParams->yawMinLimit;
        pitchMaxLimit = limiterParams->pitchMaxLimit;
        rollMaxLimit = limiterParams->rollMaxLimit;
        yawMaxLimit = limiterParams->yawMaxLimit;
    }
    else
    {
        pitchMinLimit = -180.f;
        rollMinLimit = -180.f;
        yawMinLimit = -180.f;
        pitchMaxLimit = 180.f;
        rollMaxLimit = 180.f;
        yawMaxLimit = 180.f;
    }
}

void RotationLimiterComponent::Update(float /*elapsedTime*/)
{
    auto* sceneNode = Core::Entity::GetEntity(GetEntityHandle())->GetAnchor();

    LibMath::Vector3 eulerRotation = sceneNode->GetWorldTransformNoCheck().rotation.EulerAngles();
    bool isRotationChanged = false;

    if (eulerRotation.x < pitchMinLimit)
    {
        eulerRotation.x = pitchMinLimit;
        isRotationChanged = true;
    }
    else if (eulerRotation.x > pitchMaxLimit)
    {
        eulerRotation.x = pitchMaxLimit;
        isRotationChanged = true;
    }

    if(eulerRotation.y < rollMinLimit)
    {
        eulerRotation.y = rollMinLimit;
        isRotationChanged = true;
    }
    else if (eulerRotation.y > rollMaxLimit)
    {
        eulerRotation.y = rollMaxLimit;
        isRotationChanged = true;
    }

    if (eulerRotation.z < yawMinLimit)
    {
        eulerRotation.z = yawMinLimit;
        isRotationChanged = true;
    }
    else if (eulerRotation.z > yawMaxLimit)
    {
        eulerRotation.z = yawMaxLimit;
        isRotationChanged = true;
    }

    if(isRotationChanged)
    {
        // todo fix rotation when Yaw axis is equal to 90 degrees
        //sceneNode->SetWorldRotation(LibMath::Quaternion(LibMath::Degree(eulerRotation.x), LibMath::Degree(eulerRotation.y), LibMath::Degree(eulerRotation.z)));
    }

}