#include "GLFWEncapsulation.h"

#include "GLFW/glfw3.h"


GLFW::GLFWkeyfunEncapsulation GLFW::GLFWSetKeyCallback(struct GLFWwindow* window, GLFWkeyfunEncapsulation callback)
{
	return glfwSetKeyCallback(window, callback);
}

void GLFW::GLFWSetInputMode(struct GLFWwindow* window, int mode, int value)
{
	glfwSetInputMode(window, mode, value);
}

GLFW::GLFWcursorposfunEncapsulation GLFW::GLFWSetCursorPosCallback(struct GLFWwindow* window,
                                                                   GLFWcursorposfunEncapsulation callback)
{
	return glfwSetCursorPosCallback(window, callback);
}

GLFW::GLFWscrollfunEncapsulation GLFW::GLFWSetScrollCallback(struct GLFWwindow* window,
                                                             GLFWscrollfunEncapsulation callback)
{
	return glfwSetScrollCallback(window, callback);
}

GLFW::GLFWmousebuttonfunEncapsulation GLFW::GLFWSetMouseButtonCallback(struct GLFWwindow* window,
                                                                       GLFWmousebuttonfunEncapsulation callback)
{
	return glfwSetMouseButtonCallback(window, callback);
}

void GLFW::GLFWGetCursorPos(struct GLFWwindow* window, double* xpos, double* ypos)
{
	glfwGetCursorPos(window, xpos, ypos);
}

void GLFW::GLFWPollEvents()
{
	glfwPollEvents();
}

int GLFW::GetGLFWRepeat()
{
	return GLFW_REPEAT;
}

int GLFW::GetGLFWPress()
{
	return GLFW_PRESS;
}

int GLFW::GetGLFWKeyLast()
{
	return GLFW_KEY_LAST;
}

int GLFW::GetGLFWKeyUnknown()
{
	return GLFW_KEY_UNKNOWN;
}

int GLFW::GetGLFWStickyKeys()
{
	return GLFW_STICKY_KEYS;
}

int GLFW::GetGLFWStickyMouseButtons()
{
	return GLFW_STICKY_MOUSE_BUTTONS;
}

int GLFW::GetGLFWCursor()
{
	return GLFW_CURSOR;
}

int GLFW::GetGLFWCursorDisabled()
{
	return GLFW_CURSOR_DISABLED;
}

int GLFW::GetGLFWCursorNormal()
{
	return GLFW_CURSOR_NORMAL;
}

int GLFW::GetKey(const Keycode keycode)
{
	switch (keycode)
	{
	case Keycode::KEY_SPACE: return GLFW_KEY_SPACE;
	case Keycode::KEY_APOSTROPHE: return GLFW_KEY_APOSTROPHE;
	case Keycode::KEY_COMMA: return GLFW_KEY_COMMA;
	case Keycode::KEY_MINUS: return GLFW_KEY_MINUS;
	case Keycode::KEY_PERIOD: return GLFW_KEY_PERIOD;
	case Keycode::KEY_SLASH: return GLFW_KEY_SLASH;
	case Keycode::KEY_0: return GLFW_KEY_0;
	case Keycode::KEY_1: return GLFW_KEY_1;
	case Keycode::KEY_2: return GLFW_KEY_2;
	case Keycode::KEY_3: return GLFW_KEY_3;
	case Keycode::KEY_4: return GLFW_KEY_4;
	case Keycode::KEY_5: return GLFW_KEY_5;
	case Keycode::KEY_6: return GLFW_KEY_6;
	case Keycode::KEY_7: return GLFW_KEY_7;
	case Keycode::KEY_8: return GLFW_KEY_8;
	case Keycode::KEY_9: return GLFW_KEY_9;
	case Keycode::KEY_SEMICOLON: return GLFW_KEY_SEMICOLON;
	case Keycode::KEY_EQUAL: return GLFW_KEY_EQUAL;
	case Keycode::KEY_A: return GLFW_KEY_A;
	case Keycode::KEY_B: return GLFW_KEY_B;
	case Keycode::KEY_C: return GLFW_KEY_C;
	case Keycode::KEY_D: return GLFW_KEY_D;
	case Keycode::KEY_E: return GLFW_KEY_E;
	case Keycode::KEY_F: return GLFW_KEY_F;
	case Keycode::KEY_G: return GLFW_KEY_G;
	case Keycode::KEY_H: return GLFW_KEY_H;
	case Keycode::KEY_I: return GLFW_KEY_I;
	case Keycode::KEY_J: return GLFW_KEY_J;
	case Keycode::KEY_K: return GLFW_KEY_K;
	case Keycode::KEY_L: return GLFW_KEY_L;
	case Keycode::KEY_M: return GLFW_KEY_M;
	case Keycode::KEY_N: return GLFW_KEY_N;
	case Keycode::KEY_O: return GLFW_KEY_O;
	case Keycode::KEY_P: return GLFW_KEY_P;
	case Keycode::KEY_Q: return GLFW_KEY_Q;
	case Keycode::KEY_R: return GLFW_KEY_R;
	case Keycode::KEY_S: return GLFW_KEY_S;
	case Keycode::KEY_T: return GLFW_KEY_T;
	case Keycode::KEY_U: return GLFW_KEY_U;
	case Keycode::KEY_V: return GLFW_KEY_V;
	case Keycode::KEY_W: return GLFW_KEY_W;
	case Keycode::KEY_X: return GLFW_KEY_X;
	case Keycode::KEY_Y: return GLFW_KEY_Y;
	case Keycode::KEY_Z: return GLFW_KEY_Z;
	case Keycode::KEY_LEFT_BRACKET: return GLFW_KEY_LEFT_BRACKET;
	case Keycode::KEY_BACKSLASH: return GLFW_KEY_BACKSLASH;
	case Keycode::KEY_RIGHT_BRACKET: return GLFW_KEY_RIGHT_BRACKET;
	case Keycode::KEY_GRAVE_ACCENT: return GLFW_KEY_GRAVE_ACCENT;
	case Keycode::KEY_ESCAPE: return GLFW_KEY_ESCAPE;
	case Keycode::KEY_ENTER: return GLFW_KEY_ENTER;
	case Keycode::KEY_TAB: return GLFW_KEY_TAB;
	case Keycode::KEY_BACKSPACE: return GLFW_KEY_BACKSPACE;
	case Keycode::KEY_INSERT: return GLFW_KEY_INSERT;
	case Keycode::KEY_DELETE: return GLFW_KEY_DELETE;
	case Keycode::KEY_ARROW_RIGHT: return GLFW_KEY_RIGHT;
	case Keycode::KEY_ARROW_LEFT: return GLFW_KEY_LEFT;
	case Keycode::KEY_ARROW_DOWN: return GLFW_KEY_DOWN;
	case Keycode::KEY_ARROW_UP: return GLFW_KEY_UP;
	case Keycode::KEY_PAGE_UP: return GLFW_KEY_PAGE_UP;
	case Keycode::KEY_PAGE_DOWN: return GLFW_KEY_PAGE_DOWN;
	case Keycode::KEY_HOME: return GLFW_KEY_HOME;
	case Keycode::KEY_END: return GLFW_KEY_END;
	case Keycode::KEY_CAPS_LOCK: return GLFW_KEY_CAPS_LOCK;
	case Keycode::KEY_SCROLL_LOCK: return GLFW_KEY_SCROLL_LOCK;
	case Keycode::KEY_NUM_LOCK: return GLFW_KEY_NUM_LOCK;
	case Keycode::KEY_PRINT_SCREEN: return GLFW_KEY_PRINT_SCREEN;
	case Keycode::KEY_PAUSE: return GLFW_KEY_PAUSE;
	case Keycode::FUNCTION_F1: return GLFW_KEY_F1;
	case Keycode::FUNCTION_F2: return GLFW_KEY_F2;
	case Keycode::FUNCTION_F3: return GLFW_KEY_F3;
	case Keycode::FUNCTION_F4: return GLFW_KEY_F4;
	case Keycode::FUNCTION_F5: return GLFW_KEY_F5;
	case Keycode::FUNCTION_F6: return GLFW_KEY_F6;
	case Keycode::FUNCTION_F7: return GLFW_KEY_F7;
	case Keycode::FUNCTION_F8: return GLFW_KEY_F8;
	case Keycode::FUNCTION_F9: return GLFW_KEY_F9;
	case Keycode::FUNCTION_F10: return GLFW_KEY_F10;
	case Keycode::FUNCTION_F11: return GLFW_KEY_F11;
	case Keycode::FUNCTION_F12: return GLFW_KEY_F12;
	case Keycode::NUMPAD_0: return GLFW_KEY_KP_0;
	case Keycode::NUMPAD_1: return GLFW_KEY_KP_1;
	case Keycode::NUMPAD_2: return GLFW_KEY_KP_2;
	case Keycode::NUMPAD_3: return GLFW_KEY_KP_3;
	case Keycode::NUMPAD_4: return GLFW_KEY_KP_4;
	case Keycode::NUMPAD_5: return GLFW_KEY_KP_5;
	case Keycode::NUMPAD_6: return GLFW_KEY_KP_6;
	case Keycode::NUMPAD_7: return GLFW_KEY_KP_7;
	case Keycode::NUMPAD_8: return GLFW_KEY_KP_8;
	case Keycode::NUMPAD_9: return GLFW_KEY_KP_9;
	case Keycode::NUMPAD_DECIMAL: return GLFW_KEY_KP_DECIMAL;
	case Keycode::NUMPAD_DIVIDE: return GLFW_KEY_KP_DIVIDE;
	case Keycode::NUMPAD_MULTIPLY: return GLFW_KEY_KP_MULTIPLY;
	case Keycode::NUMPAD_SUBTRACT: return GLFW_KEY_KP_SUBTRACT;
	case Keycode::NUMPAD_ADD: return GLFW_KEY_KP_ADD;
	case Keycode::NUMPAD_ENTER: return GLFW_KEY_KP_ENTER;
	case Keycode::NUMPAD_EQUAL: return GLFW_KEY_KP_EQUAL;
	case Keycode::MOD_LEFT_SHIFT: return GLFW_KEY_LEFT_SHIFT;
	case Keycode::MOD_LEFT_CONTROL: return GLFW_KEY_LEFT_CONTROL;
	case Keycode::MOD_LEFT_ALT: return GLFW_KEY_LEFT_ALT;
	case Keycode::MOD_LEFT_SUPER: return GLFW_KEY_LEFT_SUPER;
	case Keycode::MOD_RIGHT_SHIFT: return GLFW_KEY_RIGHT_SHIFT;
	case Keycode::MOD_RIGHT_CONTROL: return GLFW_KEY_RIGHT_CONTROL;
	case Keycode::MOD_RIGHT_ALT: return GLFW_KEY_RIGHT_ALT;
	case Keycode::MOD_RIGHT_SUPER: return GLFW_KEY_RIGHT_SUPER;
	}

	return GLFW_KEY_UNKNOWN;
}

int GLFW::GetMouseLeftClick()
{
	return GLFW_MOUSE_BUTTON_LEFT;
}

int GLFW::GetMouseRightClick()
{
	return GLFW_MOUSE_BUTTON_RIGHT;
}
