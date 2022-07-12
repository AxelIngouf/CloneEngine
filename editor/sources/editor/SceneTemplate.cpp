#include "SceneTemplate.h"

#include <string>

#include "physic/PhysicsShapeComponent/PhysicsMeshComponent.h"
#include "physic/PhysicsShapeComponent/PhysicsBoxComponent.h"
#include "core/GameLoop.h"
#include "core/ECS/World.h"
#include "core/InputManager/DefaultInputManager.h"
#include "core/scenegraph/SceneGraph.h"
#include "GameComponents/CheckpointComponent.h"
#include "GameComponents/HUDComponent.h"
#include "GameEntities/Car.h"
#include "GameEntities/Checkpoint.h"
#include "render/Light/LightComponent.h"
#include "render/RenderComponent/ModelComponent.h"
#include "GameComponents/PlatformSpawner.h"
#include "model/Model.h"
#include "physic/PhysicsShapeComponent/PhysicsSphereComponent.h"

void CleanScene()
{
	auto entities = Entity::GetAllEntity();

	while (entities.Next())
	{
		if (entities->GetAnchor()->GetParent() != nullptr)
			entities->GetAnchor()->Destroy();
	}

	if (!GameLoop::IsPaused())
		GameLoop::TogglePause();
}

void InitScene()
{
	DefaultDirectionLight();
	BindEscInput();

	World::GetLevel()->UpdateAll();
	//World::Start();

#ifdef _SHIPPING
	if(GameLoop::IsPaused())
		GameLoop::TogglePause();
#endif
}

void CheckpointsDemoScene()
{
	CleanScene();

	SceneNode* car = World::GetLevel()->GetRoot()->CreateChild(CreateCarEntity);
	car->SetName("Car");
	car->GetEntity()->AddComponent<HUDCheckpointsComponent>();
	car->SetWorldPosition({0, 5, 0});

	{
		// map model
		Render::ModelComponentInitParams initParams;
		const std::string mapPath = "assets/maps/modular/arena_square.obj";
		SceneNode* sceneNode = World::GetLevel()->GetRoot()->CreateChild();
		sceneNode->SetName("Scene");
		initParams.file.path = mapPath;
		sceneNode->GetEntity()->AddComponent<Render::ModelComponent>(&initParams);

		// map collisions
		Physics::PhysicsStaticBoxComponentParams mapInit;
		mapInit.halfExtents = LibMath::Vector3{100.f, 0.1f, 100.f};
		sceneNode->GetEntity()->AddComponent<Physics::PhysicsStaticBoxComponent>(&mapInit);
	}

	{
		// Checkpoints
		auto* sceneNode = World::GetLevel()->GetRoot()->CreateChild(CreateCheckpointEntity);
		sceneNode->GetEntity()->GetComponents<CheckpointComponent>()[0]->checkpointId = 0;
		sceneNode->SetPosition({-15, 0, 0});
		sceneNode->SetName("Checkpoint0");

		sceneNode = World::GetLevel()->GetRoot()->CreateChild(CreateCheckpointEntity);
		sceneNode->GetEntity()->GetComponents<CheckpointComponent>()[0]->checkpointId = 1;
		sceneNode->SetPosition({15, 0, -80});
		sceneNode->SetName("Checkpoint1");

		sceneNode = World::GetLevel()->GetRoot()->CreateChild(CreateCheckpointEntity);
		sceneNode->GetEntity()->GetComponents<CheckpointComponent>()[0]->checkpointId = 2;
		sceneNode->SetPosition({50, 0, 30});
		sceneNode->SetName("Checkpoint2");

		sceneNode = World::GetLevel()->GetRoot()->CreateChild(CreateCheckpointEntity);
		sceneNode->GetEntity()->GetComponents<CheckpointComponent>()[0]->checkpointId = 3;
		sceneNode->SetPosition({80, 0, 0});
		sceneNode->SetName("Checkpoint3");
	}

	InitScene();
}

void PlatformsDemoScene()
{
	CleanScene();
	Model::Model("assets/padoru/padoru.obj");

	SceneNode* car = World::GetLevel()->GetRoot()->CreateChild(CreateCarEntity);
	car->SetName("Car");
	car->GetEntity()->AddComponent<HUDDistanceTraveledComponent>();
	car->SetWorldPosition({0, 15, 0});

	SceneNode* sceneNode = World::GetLevel()->GetRoot()->CreateChild();
	sceneNode->SetName("PlatformSpawner");
	sceneNode->GetEntity()->AddComponent<editor::PlatformSpawner>();

	InitScene();
}

void PhysicsDemoScene()
{
	CleanScene();

	SceneNode* car = World::GetLevel()->GetRoot()->CreateChild(CreateCarEntity);
	car->SetName("Car");
	car->SetWorldPosition({0, 2, 0});

	SceneNode* sceneNode = World::GetLevel()->GetRoot()->CreateChild();
	sceneNode->SetName("Plane");

	{
		// model
		Render::ModelComponentInitParams initParams;
		initParams.file.path = "assets/cube.obj";
		sceneNode->GetEntity()->AddComponent<Render::ModelComponent>(&initParams);
		sceneNode->SetScale({1000, 1.f, 1000});

		// physics
		Physics::PhysicsStaticBoxComponentParams boxParams;
		boxParams.halfExtents = {1000, 0.5, 1000};
		sceneNode->GetEntity()->AddComponent<Physics::PhysicsStaticBoxComponent>(&boxParams);
	}

	{
		// jump
		LoadModelStaticPhysics("Jump", "assets/maps/modular/Jump_15.obj", { 10.f, 0.5f, 0.f }, { 0, 0, 0, 1 });

		// cubes
		SceneNode* cubesNode = World::GetLevel()->GetRoot()->CreateChild();
		cubesNode->SetName("Cubes");

		for (int i = 0; i < 20; i++)
		{
			for (int j = 0; j < 20; j++)
			{
				LoadModelDynamicPhysics(cubesNode, "Cube" + std::to_string(i + 3 * j), "assets/cube.obj",
				                        {static_cast<float>(i), 1.f + j, 10.f}, {0, 0, 0, 1});
			}
		}

		// balls
		SceneNode* spheresNode = World::GetLevel()->GetRoot()->CreateChild();
		spheresNode->SetName("Spheres");

		const int initialSize = 8;
		int side = initialSize;
		const float ballSize = 2.f;
		while (side > 0)
		{
			const float shift = (initialSize - side) / 2.f * ballSize;
			for (int i = 0; i < side; i++)
			{
				for (int j = 0; j < side; j++)
				{
					LoadModelSphereDynamicPhysics(spheresNode, "Ball", "assets/sphere.obj", {
						                              10.f - i * ballSize - shift,
						                              ballSize / 2 + (initialSize - side) * (ballSize * 0.6666f),
						                              -100.f - j * ballSize - shift
					                              }, (ballSize - 0.3f) / 2);
				}
			}
			side -= 2;
		}
	}

	InitScene();
}

void DefaultDirectionLight()
{
	SceneNode* sceneNode = World::GetLevel()->GetRoot()->CreateChild();
	sceneNode->SetName("Directional Light");
	Render::LightComponentInitParams initParams;

	initParams.color = {1.f, 1.f, 1.f};
	initParams.ambientWeight = 0.2f;
	initParams.diffuseWeight = 0.6f;
	initParams.specularWeight = 0.7f;

	sceneNode->SetRotation(LibMath::Quaternion(LibMath::Degree(19.f),
	                                           LibMath::Degree(-4.f),
	                                           LibMath::Degree(17.f)));

	sceneNode->GetEntity()->AddComponent<Render::DirectionalLightComponent>(&initParams);
}

void MainMenu()
{
	CleanScene();
	UnbindEscInput();

	if (!GameLoop::IsPaused())
		GameLoop::TogglePause();

	SceneNode* sceneNode = World::GetLevel()->GetRoot()->CreateChild();
	sceneNode->SetName("Main Menu");

	UIButtonParams buttonParams;
	buttonParams.size = LibMath::Vector2(400, 50);
	buttonParams.location = LibMath::Vector2(300, 150);
	buttonParams.text = "First Demo";
	auto* firstDemoButton = sceneNode->GetEntity()->AddComponent<UIButtonComponent>(&buttonParams);
	firstDemoButton->button.button->onButtonPressed.Add(CheckpointsDemoScene);

	buttonParams.location = LibMath::Vector2(300, 250);
	buttonParams.text = "Second Demo";
	auto* secondDemoButton = sceneNode->GetEntity()->AddComponent<UIButtonComponent>(&buttonParams);
	secondDemoButton->button.button->onButtonPressed.Add(PlatformsDemoScene);

	buttonParams.location = LibMath::Vector2(300, 350);
	buttonParams.text = "Physics Demo";
	auto* physicsDemoButton = sceneNode->GetEntity()->AddComponent<UIButtonComponent>(&buttonParams);
	physicsDemoButton->button.button->onButtonPressed.Add(PhysicsDemoScene);

	buttonParams.location = LibMath::Vector2(300, 450);
	buttonParams.text = "Quit";
	auto* quitButton = sceneNode->GetEntity()->AddComponent<UIButtonComponent>(&buttonParams);
	quitButton->button.button->onButtonPressed.Add([]
	{
		exit(EXIT_SUCCESS);
	});
}

bool g_pauseMenu = false;
SceneNode* pauseMenuNode = nullptr;

void PauseMenu()
{
	if (!g_pauseMenu && !GameLoop::IsPaused())
	{
		pauseMenuNode = World::GetLevel()->GetRoot()->CreateChild();
		pauseMenuNode->SetName("Pause Menu");

		UIButtonParams buttonParams;
		buttonParams.size = LibMath::Vector2(400, 50);
		buttonParams.location = LibMath::Vector2(300, 150);
		buttonParams.text = "Resume";
		auto* firstDemoButton = pauseMenuNode->GetEntity()->AddComponent<UIButtonComponent>(&buttonParams);
		firstDemoButton->button.button->onButtonPressed.Add([]
		{
			PauseMenu();
		});

		buttonParams.location = LibMath::Vector2(300, 250);
		buttonParams.text = "Main Menu";
		auto* secondDemoButton = pauseMenuNode->GetEntity()->AddComponent<UIButtonComponent>(&buttonParams);
		secondDemoButton->button.button->onButtonPressed.Add([]
		{
			pauseMenuNode = nullptr;
			MainMenu();
		});

		buttonParams.location = LibMath::Vector2(300, 350);
		buttonParams.text = "Quit";
		auto* quitButton = pauseMenuNode->GetEntity()->AddComponent<UIButtonComponent>(&buttonParams);
		quitButton->button.button->onButtonPressed.Add([]
		{
			exit(EXIT_SUCCESS);
		});

		g_pauseMenu = true;
		if (!GameLoop::IsPaused())
			GameLoop::TogglePause();
	}
	else
	{
		if (pauseMenuNode != nullptr)
		{
			pauseMenuNode->Destroy();
			pauseMenuNode = nullptr;
		}

		g_pauseMenu = false;

#ifdef _SHIPPING
		if(GameLoop::IsPaused())
			GameLoop::TogglePause();
#endif
	}
}

void BindEscInput()
{
	//DefaultInputManager::OnPressed(EActionEnum::ESCAPE)->Add(PauseMenu);
}

void UnbindEscInput()
{
	//DefaultInputManager::OnPressed(EActionEnum::ESCAPE)->ClearDelegates();
}

void LoadModelDynamicPhysics(SceneNode* sceneNode, const std::string& name, const std::string& path, const LibMath::Vector3& location,
                             const LibMath::Quaternion& rotation)
{
	SceneNode* node = sceneNode->CreateChild();
	node->SetName(name);
	node->SetPosition(location);
	node->SetRotation(rotation);

	// model
	Render::ModelComponentInitParams modelParams;
	modelParams.file.path = path;
	node->GetEntity()->AddComponent<Render::ModelComponent>(&modelParams);

	// physics
	Physics::DynamicConvexMeshParams physicsParams;
	physicsParams.file.path = path;
	node->GetEntity()->AddComponent<Physics::PhysicsDynamicConvexMeshComponent>(&physicsParams);
}

void LoadModelStaticPhysics(const std::string& name, const std::string& path, const LibMath::Vector3& location,
                            const LibMath::Quaternion& rotation)
{
	SceneNode* sceneNode = World::GetLevel()->GetRoot()->CreateChild();
	sceneNode->SetName(name);
	sceneNode->SetPosition(location);
	sceneNode->SetRotation(rotation);

	// model
	Render::ModelComponentInitParams modelParams;
	modelParams.file.path = path;
	sceneNode->GetEntity()->AddComponent<Render::ModelComponent>(&modelParams);

	// physics
	Physics::StaticConvexMeshParams physicsParams;
	physicsParams.file.path = path;
	sceneNode->GetEntity()->AddComponent<Physics::PhysicsStaticConvexMeshComponent>(&physicsParams);
}

void LoadModelSphereDynamicPhysics(SceneNode* sceneNode, const std::string& name, const std::string& path, const LibMath::Vector3& location,
                                   float sphereRadius)
{
	SceneNode* node = sceneNode->CreateChild();
	node->SetName(name);
	node->SetPosition(location);
	const float modelScale = 2.4f / 5.2f * sphereRadius;
	node->SetScale(LibMath::Vector3{modelScale});

	// model
	Render::ModelComponentInitParams modelParams;
	modelParams.file.path = path;
	node->GetEntity()->AddComponent<Render::ModelComponent>(&modelParams);

	// physics
	Physics::PhysicsDynamicSphereComponentParams physicsParams;
	physicsParams.density = 5.f;
	physicsParams.restitution = 0.1f;
	physicsParams.dynamicFriction = 0.95f;
	physicsParams.staticFriction = 0.8f;
	physicsParams.radius = sphereRadius;
	node->GetEntity()->AddComponent<Physics::PhysicsDynamicSphereComponent>(&physicsParams);
}
