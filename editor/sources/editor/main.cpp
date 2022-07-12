#include <cstdlib>


#include "SceneTemplate.h"
#include "core/GameLoop.h"
#include "core/ECS/World.h"
#include "core/reflection/template/NonRegressionTest.h"
#include "physic/PhysicsManager.h"
#include "render/Camera/FreeCam.h"
#include "sound/SoundManager.h"

int main()
{
	Core::NonRegressionReflectionTest();

	Core::InitReflection();
	Physics::InitPhysics();

	Core::World::Initialize();


	Core::GameLoop gameLoop{};

	MainMenu();

	gameLoop.Run();

	return EXIT_SUCCESS;
}
