#include "CheckpointComponent.h"

#include "CarControllerComponent.h"
#include "core/ResourceManager.h"
#include "core/ECS/Entity.h"
#include "physic/PhysicsRigidActor.h"
#include "physic/PhysicsShapeComponent/PhysicsBoxComponent.h"
#include "render/RenderComponent/ModelComponent.h"
#include "sound/Sound3D.h"

CheckpointComponent::CheckpointComponent() :
	checkpointId(-1)
{
}

void CheckpointComponent::BeginPlay()
{
	auto physicsComponents = Core::Entity::GetEntity(GetEntityHandle())->GetComponents<
		Physics::PhysicsStaticBoxComponent>();
	if (physicsComponents.Size() > 0)
	{
		physicsComponents[0]->onBeginOverlap.Add(&CheckpointComponent::CarOverlap, this);
	}
}

void CheckpointComponent::CarOverlap(Physics::PhysicsRigidActor* /*thisObject*/,
                                     Physics::PhysicsRigidActor* otherObject, Physics::SweepHit /*sweepHit*/)
{
	auto controllerComponents = Core::Entity::GetEntity(otherObject->GetPhysicsEntityHandle())->GetComponents<
		CarControllerComponent>();
	if (controllerComponents.Size() > 0)
	{
		if (checkpointId == controllerComponents[0]->GetNextCheckpointId())
		{
			CheckpointReached();

			controllerComponents[0]->CheckpointReached();

			HighlightCheckpoint(controllerComponents[0]->GetNextCheckpointId());
		}
	}
}

void CheckpointComponent::CheckpointReached()
{
	auto* entity = Core::Entity::GetEntity(GetEntityHandle());
	(void)entity;

	auto sound3D = entity->GetComponents<Sound::Sound3DComponent>();

	if (sound3D.Size() > 0)
	{
		sound3D[0]->PlaySound();
	}

	auto modelComponents = Core::Entity::GetEntity(GetEntityHandle())->GetComponents<Render::ModelComponent>();


	for (auto& model : modelComponents)
	{
		model->UpdateMaterials("completedCheckpointMat");
	}
}

void CheckpointComponent::HighlightCheckpoint(const int checkpointId)
{
	auto checkpoints = GetAll();

	while (checkpoints.Next())
	{
		if (checkpoints->checkpointId == checkpointId)
		{
			auto modelComponents =
				Core::Entity::GetEntity(checkpoints->GetEntityHandle())->GetComponents<Render::ModelComponent>();


			for(Render::ModelComponent* model : modelComponents)
			{
				model->UpdateMaterials("nextCheckpointMat");
			}

			return;
		}
	}
}
