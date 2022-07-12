#include "CarControllerComponent.h"


#include "CheckpointComponent.h"
#include "HUDComponent.h"
#include "core/ECS/Entity.h"
#include "core/InputManager/DefaultInputManager.h"
#include "physic/PhysicsShapeComponent/PhysicsVehicleComponent.h"
#include "sound/Sound3D.h"

int controllerCount = 0;

CarControllerComponent::CarControllerComponent() :
	nextCheckpointId(0),
	currentLap(0),
	totalLap(3),
	controllerId(-1)
{
}

void CarControllerComponent::BeginPlay()
{
    auto physicsComponents = Core::Entity::GetEntity(GetEntityHandle())->GetComponents<Physics::PhysicsVehicleComponent>();

	controllerId = controllerCount;
	controllerCount++;

    if(physicsComponents.Size() > 0)
    {
        auto* physicsComponent = physicsComponents[0];

		Core::DefaultInputManager::OnPressed(EActionEnum::ARROW_UP)->Add(&Physics::PhysicsVehicleComponent::MoveForward, physicsComponent, 1.f);
		Core::DefaultInputManager::OnPressed(EActionEnum::ARROW_DOWN)->Add([physicsComponent] { physicsComponent->MoveForward(-1.f); });
		Core::DefaultInputManager::OnPressed(EActionEnum::ARROW_RIGHT)->Add([physicsComponent] { physicsComponent->MoveRight(1.f); });
		Core::DefaultInputManager::OnPressed(EActionEnum::ARROW_LEFT)->Add([physicsComponent] { physicsComponent->MoveRight(-1.f); });

		Core::DefaultInputManager::OnHold(EActionEnum::ARROW_UP)->Add([physicsComponent] { physicsComponent->MoveForward(1.f); });
		Core::DefaultInputManager::OnHold(EActionEnum::ARROW_DOWN)->Add([physicsComponent] { physicsComponent->MoveForward(-1.f); });
		Core::DefaultInputManager::OnHold(EActionEnum::ARROW_RIGHT)->Add([physicsComponent] { physicsComponent->MoveRight(1.f); });
		Core::DefaultInputManager::OnHold(EActionEnum::ARROW_LEFT)->Add([physicsComponent] { physicsComponent->MoveRight(-1.f); });

		Core::DefaultInputManager::OnReleased(EActionEnum::ARROW_UP)->Add([physicsComponent] { physicsComponent->MoveForward(0.f); });
		Core::DefaultInputManager::OnReleased(EActionEnum::ARROW_DOWN)->Add([physicsComponent] { physicsComponent->MoveForward(0.f); });
		Core::DefaultInputManager::OnReleased(EActionEnum::ARROW_RIGHT)->Add([physicsComponent] { physicsComponent->MoveRight(0.f); });
		Core::DefaultInputManager::OnReleased(EActionEnum::ARROW_LEFT)->Add([physicsComponent] { physicsComponent->MoveRight(0.f); });
    }

	auto checkpointComponents = CheckpointComponent::GetAll();
	CheckpointComponent::HighlightCheckpoint(nextCheckpointId);

	while(checkpointComponents.Next())
	{
	    if(checkpointComponents->checkpointId > lastCheckpointId)
	    {
			lastCheckpointId = checkpointComponents->checkpointId;
	    }
	}

	onCheckpointReached.Broadcast(nextCheckpointId + 1, lastCheckpointId + 1);
	onLapFinished.Broadcast(currentLap + 1, totalLap);
}

int CarControllerComponent::GetControllerId()
{
    return controllerId;
}

int CarControllerComponent::GetNextCheckpointId()
{
	return nextCheckpointId;
}

void CarControllerComponent::SetNextCheckpointId(int newCheckpointId)
{
	nextCheckpointId = newCheckpointId;
}

int CarControllerComponent::GetCurrentLap()
{
	return currentLap;
}

void CarControllerComponent::SetCurrentLap(int newLap)
{
	currentLap = newLap;
}

void CarControllerComponent::CheckpointReached()
{
	nextCheckpointId++;
	if (nextCheckpointId > lastCheckpointId)
	{
		nextCheckpointId = 0;
		currentLap++;
		onLapFinished.Broadcast(currentLap + 1, totalLap);
	}

	onCheckpointReached.Broadcast(nextCheckpointId + 1, lastCheckpointId + 1);
}