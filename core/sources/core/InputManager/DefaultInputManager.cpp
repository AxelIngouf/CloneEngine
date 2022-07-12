#include "DefaultInputManager.h"

#include "core/GameLoop.h"

namespace Core
{
	InputManager<EActionEnum> DefaultInputManager::defaultInputManager;

	void DefaultInputManager::Init(GLFWwindow* window)
	{
		defaultInputManager.InitInputManager(window);

		defaultInputManager.Bind(GLFW::GetKey(GLFW::Keycode::KEY_ESCAPE), EActionEnum::ESCAPE);

		defaultInputManager.Bind(GLFW::GetKey(GLFW::Keycode::KEY_ARROW_UP), EActionEnum::ARROW_UP);
		defaultInputManager.Bind(GLFW::GetKey(GLFW::Keycode::KEY_ARROW_DOWN), EActionEnum::ARROW_DOWN);
		defaultInputManager.Bind(GLFW::GetKey(GLFW::Keycode::KEY_ARROW_LEFT), EActionEnum::ARROW_LEFT);
		defaultInputManager.Bind(GLFW::GetKey(GLFW::Keycode::KEY_ARROW_RIGHT), EActionEnum::ARROW_RIGHT);

		// Mouse
		defaultInputManager.Bind(GLFW::GetMouseLeftClick(), EActionEnum::MOUSE_LEFT_CLICK);
		defaultInputManager.Bind(GLFW::GetMouseRightClick(), EActionEnum::MOUSE_RIGHT_CLICK);

#ifndef _SHIPPING
		/* Editor*/
		// Editor Shortcuts
		defaultInputManager.Bind(GLFW::GetKey(GLFW::Keycode::FUNCTION_F5), EActionEnum::EDITOR_PAUSE);
		defaultInputManager.OnPressed(EActionEnum::EDITOR_PAUSE)->Add([] { GameLoop::TogglePause(); });
		defaultInputManager.Bind(GLFW::GetKey(GLFW::Keycode::FUNCTION_F1), EActionEnum::EDITOR_KEY_1);
#endif

		// Editor Freecam
		defaultInputManager.Bind(GLFW::GetKey(GLFW::Keycode::KEY_W), EActionEnum::FREE_CAM_FORWARD);
		defaultInputManager.Bind(GLFW::GetKey(GLFW::Keycode::KEY_A), EActionEnum::FREE_CAM_LEFT);
		defaultInputManager.Bind(GLFW::GetKey(GLFW::Keycode::KEY_S), EActionEnum::FREE_CAM_BACKWARD);
		defaultInputManager.Bind(GLFW::GetKey(GLFW::Keycode::KEY_D), EActionEnum::FREE_CAM_RIGHT);
		defaultInputManager.Bind(GLFW::GetKey(GLFW::Keycode::KEY_Z), EActionEnum::FREE_CAM_FORWARD);
		defaultInputManager.Bind(GLFW::GetKey(GLFW::Keycode::KEY_Q), EActionEnum::FREE_CAM_LEFT);
		defaultInputManager.Bind(GLFW::GetKey(GLFW::Keycode::KEY_SPACE), EActionEnum::FREE_CAM_UP);
		defaultInputManager.Bind(GLFW::GetKey(GLFW::Keycode::MOD_LEFT_CONTROL), EActionEnum::FREE_CAM_DOWN);
		defaultInputManager.Bind(GLFW::GetKey(GLFW::Keycode::MOD_LEFT_SHIFT), EActionEnum::FREE_CAM_SPRINT);

	}
}

