#pragma once
#include "core/CLog.h"
#include <queue>
#include <unordered_map>

namespace Core
{
    /**
	 * Different possible states an input can have.
	 */
	enum class EActionState
	{
		FREE,
		RELEASED,
		PRESSED,
		HOLD,
	};

    /**
	 * Input manager that wraps the GLFW input and mouse management.
	 * Allows the bind, rebind and unbind of GLFW inputs to custom made actions transmitted via the ActionEnum template parameter.
	 * 
	 * @tparam ActionEnum - Custom made enumerator containing a set of actions and need to end with a NO_ACTION named value.
	 */
	template<typename ActionEnum>
	class InputManager
	{
		using InputQueue = std::queue<std::pair<ActionEnum, EActionState>>;

	public:
		InputManager() = delete;
		explicit InputManager(GLFWwindow* window) { InitInputManager(window); }
		InputManager(InputManager const& other) = delete;
		InputManager(InputManager&& other) = delete;
		~InputManager() = default;

		InputManager& operator=(InputManager const& other) = delete;
		InputManager& operator=(InputManager&& other) = delete;

        /**
		 * Update keyboard and mouse inputs.
		 */
		void Update();

        /**
		 * Displays the mouse cursor on the screen. Or hide it if it's already displayed.
		 */
		void ToggleMouseCapture();

        /**
		 * Indicates id the mouse is currently captured by the GLFW window.
		 * 
		 * @return boolean, true if captured, false if not.
		 */
		[[nodiscard]] bool IsMouseCaptured() const { return isMouseCaptured; }

        /**
		 * Returns the current Y position of the mouse on the screen.
		 * 
		 * @return Current Y position of the mouse on the screen
		 */
		[[nodiscard]] double GetMouseVerticalMove() const { return currentMouseVerticalMove; }

		/**
		 * Returns the current X position of the mouse on the screen.
		 *
		 * @return Current X position of the mouse on the screen
		 */
		[[nodiscard]] double GetMouseHorizontalMove() const { return currentMouseHorizontalMove; }

        /**
		 * Returns the current mouse scroll position.
		 * 
		 * @return Current mouse scroll position.
		 */
		[[nodiscard]] double GetMouseScroll() const { return currentMouseScroll; }

        /**
		 * Returns the associated ActionState with the specified action.
		 * 
		 * @param action - ActionEnum action 
		 * @return State of the specified action.
		 */
		EActionState GetState(ActionEnum action) const
		{
#ifdef _DEBUG
			ASSERT(0 <= static_cast<int>(action) && static_cast<int>(action) < static_cast<int>(ActionEnum::NO_ACTION), "InputManager : Invalid Action");
#endif // _DEBUG
			return currentState[static_cast<int>(action)];
		}

        /**
		 * Finds and returns the first GLFW input bound to the specified ActionEnum action.
		 * Returns GLFW_KEY_UNKNOWN if no input was found.
		 * 
		 * @param action - ActionEnum action
		 * @return First GLFW input bound to the specified ActionEnum action
		 */
		[[nodiscard]] int FindFirstBoundInput(ActionEnum action) const;

        /**
		 * Returns the ActionEnum action bound to the specified GLFW input.
		 * Returns ActionEnum::NO_ACTION if no action was found.
		 * 
		 * @param glfwInput - GLFW input to search bound action for.
		 * @return Action bound to the specified GLFW input.
		 */
		[[nodiscard]] ActionEnum GetBoundAction(int glfwInput) const { return actions[glfwInput]; }

        /**
		 * Binds the specified GLFW input and ActionEnum action together if the input is not already bounded to another action.
		 * 
		 * @param glfwInput - GLFW input to bind to the action.
		 * @param action - ActionEnum to bind to the input.
		 * @return True if the binding was successful, false if the input was already bind to another action.
		 */
		bool Bind(int glfwInput, ActionEnum action);

        /**
		 * Binds the specified GLFW input and ActionEnum action together if the input and the action are not already bounded.
		 * 
		 * @param glfwInput - GLFW input to bind to the action.
		 * @param action - ActionEnum to bind to the input.
		 * @param refuseMultiple - If true, disallow the binding of an already bound action to another input.
		 * @return True if the binding was successful, false if the input or the action is already bounded.
		 */
		bool Bind(int glfwInput, ActionEnum action, bool refuseMultiple);

        /**
		 * Unbinds the specified GLFW input.
		 * 
		 * @param glfwInput - GLFW input to unbind.
		 * @return False if the GLFW input was not bounded to any action, true otherwise.
		 */
		bool Unbind(int glfwInput);

        /**
		 * Unbinds the specified action.
		 * 
		 * @param action - ActionEnum to unbind.
		 * @param unbindAll - boolean stating if all bounded GLFW inputs must be unbound from this action.
		 * @return True if the unbinding was successful, false if the action was not bound to any input.
		 */
		bool Unbind(ActionEnum action, bool unbindAll = false);

        /**
		 * Binds specified GLFW input and ActionEnum together without checking if they are already bounded.
		 * 
		 * @param glfwInput - GLFW input to bing to the action.
		 * @param action - ActionEnum to bind to the input.
		 */
		void Rebind(int glfwInput, ActionEnum action);

        /**
		 * Binds the specified input to the specified ActionEnum,
		 * with the possibility to chose not to bind the said ActionEnum if it's already bound to another input.
		 * 
		 * @param glfwInput - GLFW input to bind.
		 * @param action - ActionEnum to bind.
		 * @param refuseMultiple - boolean, if true, will not bind an already bounded action, if false, acts like default Rebind function.
		 * @return True if the binding was successful, false if the specified ActionEnum was already bounded to another GLFW input.
		 */
		bool Rebind(int glfwInput, ActionEnum action, bool refuseMultiple);

        /**
		 * Imports a whole set of bound actions and GLFW inputs into the current InputManager.
		 * 
		 * @param import - Vector of Inputs/Actions pairs to import.
		 */
		void ImportActionBinding(std::vector<std::pair<int, ActionEnum>> const& import);

        /**
		 * Exports all bound Input/ActionEnum of the current InputManager.
		 * 
		 * @return Vector of paired GLFW input and ActionEnum.
		 */
		[[nodiscard]] std::vector<std::pair<int, ActionEnum>> ExportActionBinding() const;

	private:
        /**
		 * Static map of all bound GLFW windows and InputManagers.
		 */
		static inline std::unordered_map<GLFWwindow*, InputManager*> s_managers;

        /**
		 * Static function returning the InputManger corresponding to the specified GLFW window.
		 * 
		 * @param window - GLFW window.
		 * @return InputManager corresponding to the specified GLFW window, can be null.
		 */
		static InputManager* GetManager(GLFWwindow* window);

        /**
		 * Adds the specified GLFW Input linked ActionEnum to the pendingChange InputQueue for it to be processed later.
		 * 
		 * @param window - GLFW window linked to the triggered input
		 * @param glfwInput - Triggered GLFW input
		 * @param glfwAction - Action state of the GLFW input (pressed, released)
		 */
		static void ActionCallback(GLFWwindow* window, int glfwInput, int glfwAction);

        /**
         * GLFW callback set with glfwSetKeyCallback function.
		 * Calls the ActionCallback function for the specified GLFW key that was triggered.
		 * 
		 * @param window - GLFW window linked to the triggered input
		 * @param key - Triggered GLFW input
		 * @param action - Input state (pressed, released)
		 */
		static void KeyCallback(GLFWwindow* window, int key, int /*scanCode*/, int action, int /*mods*/) { ActionCallback(window, key, action); }

        /**
		 * GLFW callback set with glfwSetMouseButtonCallback function.
		 * Calls the ActionCallback function for the triggered mouse click.
		 * 
		 * @param window - GLFW window linked to the mouse.
		 * @param button - Triggered mouse input.
		 * @param action - Mouse input state (pressed, released)
		 */
		static void MouseClickCallback(GLFWwindow* window, int button, int action, int /*mods*/) { ActionCallback(window, button, action); }

        /**
		 * GLFW callback set with glfwSetCursorPosCallback function.
		 * Calls the ActionCallback function for mouse movement.
		 * 
		 * @param window - GLFW window linked to the mouse.
		 * @param xPos - X position of the mouse on the screen.
		 * @param yPos - Y position of the mouse on the screen.
		 */
		static void MouseMoveCallback(GLFWwindow* window, double xPos, double yPos);

        /**
		 * GLFW callback set with glfwSetScrollCallback function.
		 * Calls the ActionCallback function for mouse scrolling.
		 * 
		 * @param window - GLFW window linked to the mouse
		 * @param xOffset - X offset of the scrolling
		 * @param yOffset - Y offset of the scrolling
		 */
		static void MouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset);

        /**
		 * Initializes InputManager with the given GLFW window, setting keys and mouse callbacks.
		 * 
		 * @param w - GLFW window that will be linked to the InputManager.
		 */
		void InitInputManager(GLFWwindow* w);

        /**
		 * Go through every set inputs to change their state when needed.
		 * When they are set as PRESSED, switch them to HOLD, and when they are set as RELEASED, switch them to FREE.
		 */
		void UpdateOldState();

        /**
		 * Apply all the pendingChange InputQueue to the corresponding inputs.
		 */
		void UpdateNewState();

        /**
		 * Updates the mouse scrolling and position variables.
		 */
		void UpdateRangeState();

        /**
		 * Pointer to the linked previously created GLFW window.
		 */
		GLFWwindow* window{};

        /**
		 * Array of ActionEnum the size of number of GLFW inputs,
		 * linking GLFW inputs to their ActionEnum. Default is NO_ACTION.
		 */
		ActionEnum	actions[GLFW_KEY_LAST];

        /**
		 * Array of states (free, pressed, released, hold) for each action of the given ActionEnum.
		 */
		EActionState	currentState[static_cast<int>(ActionEnum::NO_ACTION)]{};

        /**
		 * Current mouse scroll.
		 */
		double		currentMouseScroll = .0;

        /**
		 * Current Vertical mouse scroll.
		 */
		double		currentMouseVerticalMove = .0;

        /**
		 * Current Horizontal mouse scroll.
		 */
		double		currentMouseHorizontalMove = .0;

        /**
		 * Actions queue waiting to be processed and applied.
		 */
		InputQueue	pendingChange;

        /**
		 * Amount of mouse scrolling waiting to be applied.
		 */
		double		pendingMouseScroll = 0;

        /**
		 * Previous Y position of the mouse on the screen.
		 */
		double		mouseOldVerticalPosition = 0;

		/**
		 * Previous X position of the mouse on the screen.
		 */
		double		mouseOldHorizontalPosition = 0;

		/**
		 * New X position of the mouse on the screen.
		 */
		double		mouseNewVerticalPosition = 0;

		/**
		 * New X position of the mouse on the screen.
		 */
		double		mouseNewHorizontalPosition = 0;

        /**
		 * Indicates if the mouse is currently captured or not by the GLFW window.
		 */
		bool		isMouseCaptured = false;
	};
}

#include "InputManager.inl"