#include "FreeCam.h"

#include "core/scenegraph/SceneNode.h"

namespace Render
{
	void FreeCam::SetPosition(const LibMath::Vector3& newPos)
	{
		position = newPos;
	}

	void FreeCam::SetRotation(const LibMath::Quaternion& newRot)
	{
		rotation = newRot;
		rotationEuler = rotation.EulerAngles();

		yaw = rotation.GetYaw();
		pitch = rotation.GetPitch();

		front = rotation * LibMath::Vector3::Front;
		right = rotation * LibMath::Vector3::Right;
		up = rotation * LibMath::Vector3::Up;
	}

	LibMath::Matrix4 FreeCam::ComputeLookAt()
	{
		lookAt = LibMath::Matrix4::LookAtLh(position, position + front, up);
		return lookAt;
	}

	void FreeCam::StartMovingDir(const Moving dir)
	{
		moving = (Moving)(moving | dir);
	}

	void FreeCam::StopMovingDir(const Moving dir)
	{
		moving = (Moving)(moving ^ dir);
	}

	void FreeCam::StartSprint()
	{
		sprinting = true;
	}

	void FreeCam::StopSprint()
	{
		sprinting = false;
	}

	void FreeCam::SetMove(const Moving move)
	{
		moving = move;
	}

	void FreeCam::StopMove()
	{
		moving = NOT_MOVING;
	}

	void FreeCam::Rotate(const float x, const float y, const float time)
	{
		using namespace LibMath;

		yaw += Degree(x) * CAMERA_ROT_SPEED * time;
		pitch -= Degree(y) * CAMERA_ROT_SPEED * time;

		if (pitch > Degree(89.f))
			pitch = Degree(89.f);
		if (pitch < Degree(-89.f))
			pitch = Degree(-89.f);

		yaw.Clamp();


		rotation = Quaternion(pitch, yaw, Degree(0.f));
		rotationEuler = rotation.EulerAngles();

		front = rotation * Vector3::Front;
		right = rotation * Vector3::Right;
		up = rotation * Vector3::Up;
	}

	void FreeCam::Move(const float time)
	{
		using namespace LibMath;

		float moveSpeed = CAMERA_MOVE_SPEED;
		if (sprinting)
			moveSpeed *= CAMERA_SPRINT_MULTIPLIER;

		Vector3 direction;

		if ((moving & FRONT) ^ (moving & BACK))
		{
			if (moving & FRONT)
				direction += rotation * Vector3::Front;

			else
				direction += rotation * Vector3::Back;
		}

		if ((moving & RIGHT) ^ (moving & LEFT))
		{
			if (moving & RIGHT)
				direction += rotation * Vector3::Right;
			else
				direction += rotation * Vector3::Left;
		}

		if ((moving & UP) ^ (moving & DOWN))
		{
			if (moving & UP)
				direction += Vector3::Up;
			else
				direction += Vector3::Down;
		}

		if (direction != Vector3::Zero)
		{
			direction.Normalize();
			direction *= moveSpeed * time;
		}

		position += direction;
	}
}
