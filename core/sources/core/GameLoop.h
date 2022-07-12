#pragma once

#include <string>

#include "../../../render/sources/render/RenderInclude.h"

constexpr float MAX_FPS = 240;
constexpr float S_PER_FRAME = 1000 / MAX_FPS / 1000;
constexpr short MAX_FRAME_SKIP = 10;

namespace Core
{
	class GameLoop
	{
	public:
		GameLoop();
		virtual ~GameLoop() = default;

		static void ProcessInputs();

		static void UpdateGameplay(float deltaTime);

		static void UpdatePhysics(float delta);

		static void UpdateAnimation(const float)
		{
		}

		void Render(float deltaTime) const;

		void Run();

		static void Stop() { isRunning = false; }
		static void TogglePause();
		static bool IsPaused() { return isPause; }
		static void StopWorld();


		[[nodiscard]] Render::RenderApplication GetApp() const { return app; }


	private:
		inline static bool isRunning = true;
		static bool isPause;
		inline static float elapsedTime = 0.f;

		Render::RenderApplication app;

		inline bool static needsWorldReload = false;
	};
}
