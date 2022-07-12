#pragma once
#include "core/CLog.h"
#include "InputManager.h"

namespace Core
{
	template<typename ActionEnum>
	void InputManager<ActionEnum>::InitInputManager(GLFWwindow* w)
	{
		window = w;

		GLFW::GLFWSetKeyCallback(window, KeyCallback);
		GLFW::GLFWSetInputMode(window, GLFW::GetGLFWStickyKeys(), true);

		GLFW::GLFWSetCursorPosCallback(window, MouseMoveCallback);

		GLFW::GLFWSetScrollCallback(window, MouseScrollCallback);
		GLFW::GLFWSetMouseButtonCallback(window, MouseClickCallback);
		GLFW::GLFWSetInputMode(window, GLFW::GetGLFWStickyMouseButtons(), true);

		s_managers[window] = this;

		std::fill_n(actions, GLFW::GetGLFWKeyLast(), ActionEnum::NO_ACTION);
		std::fill_n(currentState, static_cast<int>(ActionEnum::NO_ACTION), EActionState::FREE);
	}

	template<typename ActionEnum>
	void InputManager<ActionEnum>::Update()
	{
		UpdateOldState(); // compute hold and free state

		UpdateNewState(); // compute press and release state

		UpdateRangeState(); // compute mouse move and mouse scroll value

		BroadcastDelegates();
	}

	template<typename ActionEnum>
	void InputManager<ActionEnum>::ToggleMouseCapture()
	{
		isMouseCaptured = !isMouseCaptured;
		GLFW::GLFWSetInputMode(window, GLFW::GetGLFWCursor(), isMouseCaptured ? GLFW::GetGLFWCursorDisabled() : GLFW::GetGLFWCursorNormal());

		if (!isMouseCaptured)
		{
			double xPos, yPos;
			GLFW::GLFWGetCursorPos(window, &xPos, &yPos);

			mouseNewHorizontalPosition = xPos;
			mouseOldHorizontalPosition = xPos;

			mouseNewVerticalPosition = yPos;
			mouseOldVerticalPosition = yPos;
		}
	}

	template <typename ActionEnum>
	void InputManager<ActionEnum>::GetMousePosition(double* xPos, double* yPos) const
	{
			GLFW::GLFWGetCursorPos(window, xPos, yPos);
	}

	template<typename ActionEnum>
	int InputManager<ActionEnum>::FindFirstBoundInput(const ActionEnum action) const
	{
		for (int index = 0; index < GLFW::GetGLFWKeyLast(); index++)
		{
			if (actions[index] == action)
			{
				return index;
			}
		}

		return GLFW::GetGLFWKeyUnknown();
	}

	template<typename ActionEnum>
	bool InputManager<ActionEnum>::Bind(const int glfwInput, const ActionEnum action)
	{
		if (actions[glfwInput] == ActionEnum::NO_ACTION)
		{
			actions[glfwInput] = action;
			return true;
		}
		else
		{
			return false;
		}
	}

	template<typename ActionEnum>
	bool InputManager<ActionEnum>::Bind(const int glfwInput, const ActionEnum action, const bool refuseMultiple)
	{
		if (refuseMultiple && FindFirstBoundInput(action) != GLFW::GetGLFWKeyUnknown())
		{
			return false;
		}

		return Bind(glfwInput, action);
	}

	template<typename ActionEnum>
	bool InputManager<ActionEnum>::Unbind(const int glfwInput)
	{
		const bool result = actions[glfwInput] != ActionEnum::NO_ACTION;
		actions[glfwInput] = ActionEnum::NO_ACTION;
		return result;
	}

	template<typename ActionEnum>
	bool InputManager<ActionEnum>::Unbind(const ActionEnum action, const bool unbindAll)
	{
		bool result = false;

		for (int index = 0; index < GLFW::GetGLFWKeyLast(); index++)
		{
			if (actions[index] == action)
			{
				actions[index] = ActionEnum::NO_ACTION;
				if (unbindAll)
				{
					result = true;
				}
				else
				{
					return true;
				}
			}
		}

		return result;
	}

	template<typename ActionEnum>
	void InputManager<ActionEnum>::Rebind(const int glfwInput, const ActionEnum action)
	{
		actions[glfwInput] = action;
	}

	template<typename ActionEnum>
	bool InputManager<ActionEnum>::Rebind(const int glfwInput, ActionEnum action, const bool refuseMultiple)
	{
		if (refuseMultiple && FindFirstBoundInput(action) != GLFW::GetGLFWKeyUnknown())
		{
			return false;
		}

		Rebind(glfwInput, action);
		return true;
	}

	template<typename ActionEnum>
	OnKeyAction* InputManager<ActionEnum>::OnPressed(ActionEnum action)
	{
		return &onPressedDelegates[static_cast<int>(action)];
	}

	template<typename ActionEnum>
	OnKeyAction* InputManager<ActionEnum>::OnHold(ActionEnum action)
	{
		return &onHoldDelegates[static_cast<int>(action)];
	}

	template<typename ActionEnum>
	OnKeyAction* InputManager<ActionEnum>::OnReleased(ActionEnum action)
	{
		return &onReleasedDelegates[static_cast<int>(action)];
	}

	template<typename ActionEnum>
	void InputManager<ActionEnum>::ImportActionBinding(std::vector<std::pair<int, ActionEnum>> const& import)
	{
		std::fill_n(actions, GLFW::GetGLFWKeyLast(), ActionEnum::NO_ACTION);
		for (std::pair<int, ActionEnum> binding : import)
		{
			Rebind(binding.first, binding.second);
		}
	}

	template<typename ActionEnum>
	std::vector<std::pair<int, ActionEnum>> InputManager<ActionEnum>::ExportActionBinding() const
	{
		std::vector<std::pair<int, ActionEnum>> result;

		for (int index = 0; index < GLFW::GetGLFWKeyLast(); index++)
		{
			if (actions[index] != ActionEnum::NO_ACTION)
			{
				result.emplace_back(index, actions[index]);
			}
		}

		return std::move(result);
	}

	template<typename ActionEnum>
	InputManager<ActionEnum>* InputManager<ActionEnum>::GetManager(GLFWwindow* window)
	{
		typename std::unordered_map<GLFWwindow*, InputManager*>::iterator findResult = s_managers.find(window);

		if (findResult != s_managers.end())
		{
			return findResult->second;
		}

		LOG(LOG_WARNING, "Current window has no InputManager attached.");
		return nullptr;
	}

	template<typename ActionEnum>
	void InputManager<ActionEnum>::ActionCallback(GLFWwindow* window, const int glfwInput, const int glfwAction)
	{

		// GLFW_REPEAT use OS (windows) event and is triggered less than once a couple of frame. So this InputManager will compute the hold action instead.
		if (glfwAction == GLFW::GetGLFWRepeat())
		{
			return;
		}

		if (glfwInput == GLFW::GetGLFWKeyUnknown())
		{
			return;
		}

		InputManager* manager = GetManager(window);

		ActionEnum action = manager->actions[glfwInput];

		EActionState state = glfwAction == GLFW::GetGLFWPress() ? EActionState::PRESSED : EActionState::RELEASED;

		manager->pendingChange.emplace(action, state);
	}

	template<typename ActionEnum>
	void InputManager<ActionEnum>::MouseMoveCallback(GLFWwindow* window, const double xPos, const double yPos)
	{
		InputManager* manager = GetManager(window);

		if (manager)
		{
			manager->mouseNewHorizontalPosition = xPos;
			manager->mouseNewVerticalPosition = yPos;
		}
	}

	template<typename ActionEnum>
	void InputManager<ActionEnum>::MouseScrollCallback(GLFWwindow* window, const double /*xOffset*/, const double yOffset)
	{
		InputManager* manager = GetManager(window);

		if (manager)
		{
			manager->pendingMouseScroll += yOffset;
		}
	}

	template<typename ActionEnum>
	void InputManager<ActionEnum>::UpdateOldState()
	{
		for (int i = 0; i < static_cast<int>(ActionEnum::NO_ACTION); i++)
		{
			if (currentState[i] == EActionState::PRESSED)
			{
				currentState[i] = EActionState::HOLD;
			}
			else if (currentState[i] == EActionState::RELEASED)
			{
				currentState[i] = EActionState::FREE;
			}
		}
	}

	template<typename ActionEnum>
	void InputManager<ActionEnum>::UpdateNewState()
	{
		while (!pendingChange.empty())
		{
			std::pair<ActionEnum, EActionState> pair = pendingChange.front();
			currentState[static_cast<int>(pair.first)] = pair.second;
			pendingChange.pop();
		}
	}

	template<typename ActionEnum>
	void InputManager<ActionEnum>::UpdateRangeState()
	{
		currentMouseHorizontalMove = mouseNewHorizontalPosition - mouseOldHorizontalPosition;
		mouseOldHorizontalPosition = mouseNewHorizontalPosition;

		currentMouseVerticalMove = mouseOldVerticalPosition - mouseNewVerticalPosition;
		mouseOldVerticalPosition = mouseNewVerticalPosition;

		currentMouseScroll = pendingMouseScroll;
		pendingMouseScroll = 0;
	}

	template <typename ActionEnum>
	void InputManager<ActionEnum>::BroadcastDelegates()
	{
		for (int iAction = 0; iAction < static_cast<int>(ActionEnum::NO_ACTION); iAction++)
		{
			switch (currentState[iAction])
			{
			case EActionState::PRESSED:
				onPressedDelegates[iAction].Broadcast();
				break;
			case EActionState::HOLD:
				onHoldDelegates[iAction].Broadcast();
				break;
			case EActionState::RELEASED:
				onReleasedDelegates[iAction].Broadcast();
				break;
			default:
				break;
			}
		}
	}

}
