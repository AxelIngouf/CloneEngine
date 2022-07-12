#pragma once
#include "../CLog.h"
#include "../Delegate.h"
#include <queue>
#include <unordered_map>

#include "../../../../render/sources/glfw/GLFWEncapsulation.h"

DELEGATE(KeyAction);

DELEGATE_One_Param(InputValue, double, value); // todo use this for analog inputs

#define GLFW_KEY_LAST_FORWARD_DECLARATION 348

namespace Core
{
	enum class EActionState
	{
		FREE,
		RELEASED,
		PRESSED,
		HOLD,
	};

	template <typename ActionEnum>
	class InputManager
	{
		using InputQueue = std::queue<std::pair<ActionEnum, EActionState>>;

	public:
		InputManager() = default;
		explicit InputManager(GLFWwindow* window) { InitInputManager(window); }
		InputManager(InputManager const& other) = delete;
		InputManager(InputManager&& other) = delete;
		~InputManager() = default;

		InputManager& operator=(InputManager const& other) = delete;
		InputManager& operator=(InputManager&& other) = delete;

		void InitInputManager(GLFWwindow* w);

		void Update();

		void ToggleMouseCapture();
		[[nodiscard]] bool IsMouseCaptured() const { return isMouseCaptured; }

		[[nodiscard]] double GetMouseVerticalMove() const { return currentMouseVerticalMove; }
		[[nodiscard]] double GetMouseHorizontalMove() const { return currentMouseHorizontalMove; }
		[[nodiscard]] double GetMouseScroll() const { return currentMouseScroll; }
		void GetMousePosition(double* xPos, double* yPos) const;

		[[nodiscard]] EActionState GetState(ActionEnum action) const
		{
#ifdef _DEBUG
			ASSERT(0 <= static_cast<int>(action) && static_cast<int>(action) < static_cast<int>(ActionEnum::NO_ACTION),
			       "InputManager : Invalid Action");
#endif // _DEBUG
			return currentState[static_cast<int>(action)];
		}

		[[nodiscard]] int FindFirstBoundInput(ActionEnum action) const;
		[[nodiscard]] ActionEnum GetBoundAction(int glfwInput) const { return actions[glfwInput]; }
		bool Bind(int glfwInput, ActionEnum action);
		bool Bind(int glfwInput, ActionEnum action, bool refuseMultiple);
		bool Unbind(int glfwInput);
		bool Unbind(ActionEnum action, bool unbindAll = false);
		void Rebind(int glfwInput, ActionEnum action);
		bool Rebind(int glfwInput, ActionEnum action, bool refuseMultiple);

		OnKeyAction* OnPressed(ActionEnum action);
		OnKeyAction* OnHold(ActionEnum action);
		OnKeyAction* OnReleased(ActionEnum action);

		void ImportActionBinding(std::vector<std::pair<int, ActionEnum>> const& import);
		[[nodiscard]] std::vector<std::pair<int, ActionEnum>> ExportActionBinding() const;

	private:
		static inline std::unordered_map<GLFWwindow*, InputManager*> s_managers; // TODO: change s_managers to vector
		static InputManager* GetManager(GLFWwindow* window);
		static void ActionCallback(GLFWwindow* window, int glfwInput, int glfwAction);

		static void KeyCallback(GLFWwindow* window, int key, int /*scanCode*/, int action, int /*mods*/)
		{
			ActionCallback(window, key, action);
		}

		static void MouseClickCallback(GLFWwindow* window, int button, int action, int /*mods*/)
		{
			ActionCallback(window, button, action);
		}

		static void MouseMoveCallback(GLFWwindow* window, double xPos, double yPos);
		static void MouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset);

		void UpdateOldState();
		void UpdateNewState();
		void UpdateRangeState();
		void BroadcastDelegates();

		GLFWwindow* window{};
		ActionEnum actions[GLFW_KEY_LAST_FORWARD_DECLARATION]{};
		OnKeyAction onPressedDelegates[static_cast<int>(ActionEnum::NO_ACTION)]{};
		OnKeyAction onHoldDelegates[static_cast<int>(ActionEnum::NO_ACTION)]{};
		OnKeyAction onReleasedDelegates[static_cast<int>(ActionEnum::NO_ACTION)]{};

		EActionState currentState[static_cast<int>(ActionEnum::NO_ACTION)]{};
		double currentMouseScroll = .0;
		double currentMouseVerticalMove = .0;
		double currentMouseHorizontalMove = .0;

		InputQueue pendingChange;
		double pendingMouseScroll = 0;
		double mouseOldVerticalPosition = 0;
		double mouseOldHorizontalPosition = 0;
		double mouseNewVerticalPosition = 0;
		double mouseNewHorizontalPosition = 0;

		bool isMouseCaptured = false;
	};
}


#include "InputManager.inl"
