#pragma once
#include "InputManager.h"

enum class EActionEnum
{
	ARROW_UP,
	ARROW_DOWN,
	ARROW_LEFT,
	ARROW_RIGHT,
	FREE_CAM_FORWARD,
	FREE_CAM_BACKWARD,
	FREE_CAM_LEFT,
	FREE_CAM_RIGHT,
	FREE_CAM_UP,
	FREE_CAM_DOWN,
	FREE_CAM_SPRINT,
	MOUSE_LEFT_CLICK,
	MOUSE_RIGHT_CLICK,
	EDITOR_PAUSE,
	EDITOR_KEY_1,
	ESCAPE,

	NO_ACTION
};

namespace Core
{
	class DefaultInputManager
	{
	public:
		static void Init(struct GLFWwindow* window);

		static OnKeyAction* OnPressed(const EActionEnum& action) { return defaultInputManager.OnPressed(action); }
		static OnKeyAction* OnHold(const EActionEnum& action) { return defaultInputManager.OnHold(action); }
		static OnKeyAction* OnReleased(const EActionEnum& action) { return defaultInputManager.OnReleased(action); }

		static EActionState GetActionState(const EActionEnum& action) { return defaultInputManager.GetState(action); }
		static double GetMouseHorizontalMove() { return defaultInputManager.GetMouseHorizontalMove(); }
		static double GetMouseVerticalMove() { return defaultInputManager.GetMouseVerticalMove(); }
		static void ToggleMouseCapture() { defaultInputManager.ToggleMouseCapture(); }
		static void GetMousePosition(double* xPos, double* yPos) { defaultInputManager.GetMousePosition(xPos, yPos); }

		static void ProcessInputs() { defaultInputManager.Update(); }

	private:
		DefaultInputManager() = default;

		static InputManager<EActionEnum> defaultInputManager;
	};
}
