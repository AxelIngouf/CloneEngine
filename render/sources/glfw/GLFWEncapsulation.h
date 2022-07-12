#pragma once
#include "core/Delegate.h"

struct GLFWwindow;

class GLFW
{
public:
	/* GLFW Constants getters */
	static int GetGLFWRepeat();
	static int GetGLFWPress();
	static int GetGLFWKeyLast();
	static int GetGLFWKeyUnknown();
	static int GetGLFWStickyKeys();
	static int GetGLFWStickyMouseButtons();
	static int GetGLFWCursor();
	static int GetGLFWCursorDisabled();
	static int GetGLFWCursorNormal();

	enum class Keycode
	{
		KEY_SPACE,
		KEY_APOSTROPHE,
		KEY_COMMA,
		KEY_MINUS,
		KEY_PERIOD,
		KEY_SLASH,
		KEY_0,
		KEY_1,
		KEY_2,
		KEY_3,
		KEY_4,
		KEY_5,
		KEY_6,
		KEY_7,
		KEY_8,
		KEY_9,
		KEY_SEMICOLON,
		KEY_EQUAL,
		KEY_A,
		KEY_B,
		KEY_C,
		KEY_D,
		KEY_E,
		KEY_F,
		KEY_G,
		KEY_H,
		KEY_I,
		KEY_J,
		KEY_K,
		KEY_L,
		KEY_M,
		KEY_N,
		KEY_O,
		KEY_P,
		KEY_Q,
		KEY_R,
		KEY_S,
		KEY_T,
		KEY_U,
		KEY_V,
		KEY_W,
		KEY_X,
		KEY_Y,
		KEY_Z,
		KEY_LEFT_BRACKET,
		KEY_BACKSLASH,
		KEY_RIGHT_BRACKET,
		KEY_GRAVE_ACCENT,
		KEY_ESCAPE,
		KEY_ENTER,
		KEY_TAB,
		KEY_BACKSPACE,
		KEY_INSERT,
		KEY_DELETE,
		KEY_ARROW_RIGHT,
		KEY_ARROW_LEFT,
		KEY_ARROW_DOWN,
		KEY_ARROW_UP,
		KEY_PAGE_UP,
		KEY_PAGE_DOWN,
		KEY_HOME,
		KEY_END,
		KEY_CAPS_LOCK,
		KEY_SCROLL_LOCK,
		KEY_NUM_LOCK,
		KEY_PRINT_SCREEN,
		KEY_PAUSE,
		FUNCTION_F1,
		FUNCTION_F2,
		FUNCTION_F3,
		FUNCTION_F4,
		FUNCTION_F5,
		FUNCTION_F6,
		FUNCTION_F7,
		FUNCTION_F8,
		FUNCTION_F9,
		FUNCTION_F10,
		FUNCTION_F11,
		FUNCTION_F12,
		NUMPAD_0,
		NUMPAD_1,
		NUMPAD_2,
		NUMPAD_3,
		NUMPAD_4,
		NUMPAD_5,
		NUMPAD_6,
		NUMPAD_7,
		NUMPAD_8,
		NUMPAD_9,
		NUMPAD_DECIMAL,
		NUMPAD_DIVIDE,
		NUMPAD_MULTIPLY,
		NUMPAD_SUBTRACT,
		NUMPAD_ADD,
		NUMPAD_ENTER,
		NUMPAD_EQUAL,
		MOD_LEFT_SHIFT,
		MOD_LEFT_CONTROL,
		MOD_LEFT_ALT,
		MOD_LEFT_SUPER,
		MOD_RIGHT_SHIFT,
		MOD_RIGHT_CONTROL,
		MOD_RIGHT_ALT,
		MOD_RIGHT_SUPER
	};


	static int GetKey(Keycode keycode);
	static int GetMouseLeftClick();
	static int GetMouseRightClick();


	/* GLFW Functions Encapsulation */
	typedef void (*GLFWkeyfunEncapsulation)(GLFWwindow*, int, int, int, int);
	typedef void (*GLFWcursorposfunEncapsulation)(GLFWwindow*, double, double);
	typedef void (*GLFWscrollfunEncapsulation)(GLFWwindow*, double, double);
	typedef void (*GLFWmousebuttonfunEncapsulation)(GLFWwindow*, int, int, int);
	typedef void(*GLFWwindowmaximizefun) (GLFWwindow*, int);

	static GLFWkeyfunEncapsulation GLFWSetKeyCallback(GLFWwindow* window, GLFWkeyfunEncapsulation callback);

	static void GLFWSetInputMode(GLFWwindow* window, int mode, int value);
	static GLFWcursorposfunEncapsulation GLFWSetCursorPosCallback(GLFWwindow* window,
	                                                              GLFWcursorposfunEncapsulation callback);
	static GLFWscrollfunEncapsulation GLFWSetScrollCallback(GLFWwindow* window, GLFWscrollfunEncapsulation callback);
	static GLFWmousebuttonfunEncapsulation GLFWSetMouseButtonCallback(GLFWwindow* window,
	                                                                  GLFWmousebuttonfunEncapsulation callback);


	static void GLFWGetCursorPos(GLFWwindow* window, double* xpos, double* ypos);

	static void GLFWPollEvents();
};
