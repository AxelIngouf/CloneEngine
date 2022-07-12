#pragma once

#include "core/ECS/Component.h"
#include "Vector/Vector3.h"
#include "Matrix/Matrix4.h"

COMPONENT(CameraComponent,
	FUNCTION(void, Update, float, deltaTime),
	FUNCTION(LibMath::Vector3, GetFront),
	FUNCTION(LibMath::Vector3, GetRight),
	FUNCTION(LibMath::Vector3, GetUp),
	FUNCTION(LibMath::Vector3, GetPosition),
	FUNCTION(LibMath::Quaternion, GetRotation),
	FUNCTION(LibMath::Matrix4, ComputeLookAt),
	FUNCTION(void, SetPosition, LibMath::Vector3, position),
	FUNCTION(void, SetRotation, float, pitch, float, yaw, float, roll),
	FUNCTION(void, SetRotationQuaternion, LibMath::Quaternion, rotation),
    PRIVATE_FIELD(LibMath::Vector3, front, Core::FieldFlag::READONLY),
	PRIVATE_FIELD(LibMath::Vector3, right, Core::FieldFlag::READONLY),
	PRIVATE_FIELD(LibMath::Vector3, up, Core::FieldFlag::READONLY)
);
