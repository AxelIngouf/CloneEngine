#include "GameLoop.h"
#include <chrono>


#include "TimerManager.h"
#include "../../../physic/sources/physic/PhysicsManager.h"
#include "../../../render/sources/imgui/UIDebugWindow.h"
#include "../../../sound/sources/sound/SoundManager.h"
#include "InputManager/DefaultInputManager.h"
#include "ECS/World.h"
#include "scenegraph/SceneGraph.h"

namespace Core
{
	bool GameLoop::isPause = true;

	GameLoop::GameLoop() : app(1600, 900, "Clone Engine")
	{
	}

	void GameLoop::ProcessInputs()
	{
		GLFW::GLFWPollEvents();
		DefaultInputManager::ProcessInputs();
	}

	void GameLoop::UpdateGameplay(const float deltaTime)
	{
		TimerManager::GetTimerManager().Tick(deltaTime);
		World::UpdateAll(deltaTime);
		Sound::SoundManager::Update(deltaTime);
	}

	void GameLoop::UpdatePhysics(const float delta)
	{
		Physics::Advance(delta);
	}

	void GameLoop::Render(const float deltaTime) const
	{
		app.DrawFrame(deltaTime);
	}

	void GameLoop::Run()
	{
		auto start = std::chrono::system_clock::now();
		float lag = 0.0;

#ifdef _SHIPPING
		isPause = false;
#endif

		while (isRunning)
		{
			if (needsWorldReload)
			{
				World::Stop();
				needsWorldReload = false;
			}

			START_BENCHMARK("Frame time");

			auto current = std::chrono::system_clock::now();
			elapsedTime = std::chrono::duration<float>(current - start).count();
			start = current;

			ProcessInputs();

			if (!isPause)
			{
				lag += elapsedTime;

				short loopCount = 0;
				while (lag >= S_PER_FRAME)
				{
					if (loopCount < MAX_FRAME_SKIP)
					{
						UpdatePhysics(S_PER_FRAME);
						loopCount++;
					}
					lag -= S_PER_FRAME;
				}

				UpdateGameplay(elapsedTime);

				UpdateAnimation(elapsedTime);
			}

			// Deep clean world transform
			World::GetLevel()->UpdateAll();

			Render(elapsedTime);
			STOP_BENCHMARK("Frame time");
		}
	}

	void GameLoop::TogglePause()
	{
		isPause = !isPause;


		if (!isPause)
		{
			if (World::HasStarted())
				World::UnPause();
			else
				World::Start();
		}
		else
		{
			World::Pause();
		}
	}

	void GameLoop::StopWorld()
	{
		isPause = true;
		needsWorldReload = true;
	}
}
