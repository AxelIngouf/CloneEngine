#include "CameraComponent.h"

#include "core/ECS/Entity.h"
#include "core/scenegraph/SceneNode.h"

void CameraComponent::Update(float /*deltaTime*/)
{
	const LibMath::Quaternion rotation =
		Core::Entity::GetEntity(GetEntityHandle())->GetAnchor()->GetWorldTransformCheck().rotation;

	front = rotation * LibMath::Vector3::Front;
	right = rotation * LibMath::Vector3::Right;
	up = rotation * LibMath::Vector3::Up;
}

 LibMath::Vector3 CameraComponent::GetFront()
{
    return front;
}

 LibMath::Vector3 CameraComponent::GetRight()
{
    return right;
}

 LibMath::Vector3 CameraComponent::GetUp()
{
    return up;
}

 LibMath::Vector3 CameraComponent::GetPosition()
{
	return Core::Entity::GetEntity(GetEntityHandle())->GetAnchor()->GetWorldTransformCheck().position;
}

 LibMath::Quaternion CameraComponent::GetRotation()
{
	return Core::Entity::GetEntity(GetEntityHandle())->GetAnchor()->GetWorldTransformCheck().rotation;
}

void CameraComponent::SetPosition( LibMath::Vector3 position)
{
	Core::Entity::GetEntity(GetEntityHandle())->GetAnchor()->SetPosition(position);
}

void CameraComponent::SetRotation(const float pitch, const float yaw, const float roll)
{
	Core::Entity::GetEntity(GetEntityHandle())->GetAnchor()->SetRotation(
		LibMath::Quaternion(LibMath::Degree(pitch), LibMath::Degree(yaw), LibMath::Degree(roll)));
}

void CameraComponent::SetRotationQuaternion( LibMath::Quaternion rotation)
{
	Core::Entity::GetEntity(GetEntityHandle())->GetAnchor()->SetRotation(rotation);
}

LibMath::Matrix4 CameraComponent::ComputeLookAt()
{
	const LibMath::Vector3 position =
		Core::Entity::GetEntity(GetEntityHandle())->GetAnchor()->GetWorldTransformNoCheck().position;

	return LibMath::Matrix4::LookAtLh(position, position + front, up);
}
