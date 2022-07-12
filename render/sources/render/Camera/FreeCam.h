#pragma once

#include "Quaternion/Quaternion.h"
#include "Vector/Vector3.h"
#include "Matrix/Matrix4.h"

//todo config
#define CAMERA_ROT_SPEED 5.f
#define CAMERA_MOVE_SPEED 5.f
#define CAMERA_SPRINT_MULTIPLIER 3.f

struct CameraComponent;

namespace Render
{
	class FreeCam
	{
	public:

		enum Moving
		{
			NOT_MOVING = 0x00,
			UP = 0x01,
			DOWN = 0x02,
			LEFT = 0x04,
			RIGHT = 0x08,
			BACK = 0x10,
			FRONT = 0x20
		};

		void SetPosition(const LibMath::Vector3& newPos);
		void SetRotation(const LibMath::Quaternion& newRot);

		[[nodiscard]] const float& GetPitch() const { return pitch.data; }
		[[nodiscard]] const float& GetYaw() const { return yaw.data; }
		[[nodiscard]] const LibMath::Vector3& GetFront() const { return front; }
		[[nodiscard]] const LibMath::Vector3& GetRight() const { return right; }
		[[nodiscard]] const LibMath::Vector3& GetUp() const { return up; }
		[[nodiscard]] const LibMath::Vector3& GetPosition() const { return position; }
		[[nodiscard]] const LibMath::Quaternion& GetRotation() const { return rotation; }
		[[nodiscard]] const LibMath::Vector3& GetRotationEuler() const { return rotationEuler; }
		[[nodiscard]] const LibMath::Matrix4& GetLookAt() const { return lookAt; }

		[[nodiscard]] LibMath::Matrix4 ComputeLookAt();

		void StartMovingDir(Moving dir);
		void StopMovingDir(Moving dir);
		void SetMove(Moving move);
		void StopMove();

		void StartSprint();
		void StopSprint();

		void Rotate(float x, float y, float time);
		void Move(float time);

	private:

		LibMath::Degree pitch;
		LibMath::Degree yaw;

		LibMath::Vector3 position;
		LibMath::Vector3 rotationEuler;
		LibMath::Quaternion rotation;

		LibMath::Vector3 front = LibMath::Vector3::Front;
		LibMath::Vector3 right = LibMath::Vector3::Right;
		LibMath::Vector3 up = LibMath::Vector3::Up;

		LibMath::Matrix4 lookAt;

		Moving moving = NOT_MOVING;
		bool sprinting = false;
	};
}
