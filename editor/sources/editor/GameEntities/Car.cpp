#include "Car.h"

#include "physic/PhysicsShapeComponent/PhysicsVehicleComponent.h"
#include <core/ECS/Entity.cpp>

#include "../GameComponents/CarChassisComponent.h"
#include "../GameComponents/CarControllerComponent.h"
#include "../GameComponents/CarWheelComponent.h"
#include "../GameComponents/HUDComponent.h"
#include "editor/GameComponents/RotationLimiterComponent.h"
#include "render/Camera/CameraComponent.h"
#include "render/RenderComponent/ModelComponent.h"

const Core::Entity* CreateCarEntity(Core::SceneNode* entityAnchor)
{
	const Core::Entity* car = Core::Entity::CreateEntity(entityAnchor);

	Physics::PhysicsVehicleComponentParams vehicleParams;
	auto* comp1 = car->AddComponent<Physics::PhysicsVehicleComponent>(&vehicleParams);
	(void)comp1;

	// car controller
	auto* carControllerComponent = car->AddComponent<CarControllerComponent>();
	(void)carControllerComponent;

	auto* hud = car->AddComponent<HUDSpeedComponent>();
	(void)hud;

    const Core::EntityHandle carHandle = car->GetHandle();

	// chassis
	const auto* chassisEntity = entityAnchor->CreateChild()->GetEntity();
	chassisEntity->SetName("Chassis");
	chassisEntity->GetAnchor()->SetScale({ 1.5f, 1.5f, 1.5f });
	Render::ModelComponentInitParams initParams;
	initParams.file.path = "assets/car/Car_Body.fbx";
	auto* chassis = chassisEntity->AddComponent<Render::ModelComponent>(&initParams);
	(void)chassis;
	auto* chassisComponent = chassisEntity->AddComponent<CarChassisComponent>();
	(void)chassisComponent;

	// wheels
	initParams.file.path = "assets/car/Car_Tire.fbx";
	for(int i = 0; i < 4; i++)
	{
        const auto* wheelEntity = entityAnchor->CreateChild()->GetEntity();
		wheelEntity->SetName("Wheel" + std::to_string(i));
		wheelEntity->GetAnchor()->SetScale({ 1.5f, 1.5f, 1.5f });

	    auto* wheel = wheelEntity->AddComponent<Render::ModelComponent>(&initParams);
	    (void)wheel;
		auto* wheelComponent = wheelEntity->AddComponent<CarWheelComponent>();
		(void)wheelComponent;
	}

	// camera
	auto* cameraEntity = entityAnchor->CreateChild();
	cameraEntity->SetName("Camera");
	cameraEntity->SetPosition({ 0.f, 4.f, -15.f, });
	cameraEntity->SetRotation({ LibMath::Degree(10), LibMath::Degree(0), LibMath::Degree(0) });
	cameraEntity->GetEntity()->AddComponent<CameraComponent>();

	// camera arm (rotation limiter)
	RotationLimiterComponentParams cameraArmParams;
	cameraArmParams.pitchMaxLimit = 22.f;
	cameraArmParams.pitchMinLimit = 18.f;
	//cameraArmParams.rollMaxLimit = 10.f;
	//cameraArmParams.rollMinLimit = -10.f;
	cameraEntity->GetEntity()->AddComponent<RotationLimiterComponent>(&cameraArmParams);

	return Core::Entity::GetEntity(carHandle);
}
