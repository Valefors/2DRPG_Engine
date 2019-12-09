#include <iostream>
#include <vector>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "maths.h"
#include "globals.h"
#include "str.h"
#include "user_input.h"
#include "window.h"

void glfwErrorCallback(int errorId, const char *description)
{
    std::cout << "GLFW error n°" << errorId << " : \"" << description << "\".\n";
	exit(EXIT_FAILURE);
}

ivec2 windowDimensions;
mat4 projection;

void glfwResizeCallback(GLFWwindow* window, int width, int height)
{
    // Create a viewport in the window with adapted dimensions
    windowDimensions.x = width;
    windowDimensions.y = height;
    glViewport(0.0f, 0.0f, windowDimensions.x, windowDimensions.y);
	projection = mat4_ortho(0.0f, (float)(windowDimensions.x), (float)(windowDimensions.y), 0.0f);
}

static bool inputKeysBefore[512] = {0};
static bool inputKeysNow[512] = {0};

void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // If a key is pressed it is active while it isn't released
    if (action == GLFW_PRESS)
    {
        inputKeysNow[key] = true;
    }
    else if (action == GLFW_RELEASE)
    {
        inputKeysNow[key] = false;
    }
}

static bool firstMouseCallback = true;

ivec2 mousePosition = {0, 0};
ivec2 mouseOffset = {0, 0};

void glfwMouseCallback(GLFWwindow* window, double newX, double newY)
{
    // To avoid a first huge mouse movement when the mouse enters the window, we get the offset after the second frame
    if(firstMouseCallback)
    {
        mousePosition.x = newX;
        mousePosition.y = newY;
        firstMouseCallback = false;
    }

    mouseOffset.x = newX - mousePosition.x;
    mouseOffset.y = mousePosition.y - newY;

    mousePosition.x = newX;
    mousePosition.y = newY;
}

void glfwScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{

}

float deltaTime = 0.0f;
float currentFrameTime;
float lastFrameTime = 0.0f;

static void update_delta_time()
{
	currentFrameTime = glfwGetTime();
	deltaTime = currentFrameTime - lastFrameTime;
	lastFrameTime = currentFrameTime;
}

static void is_input_key_supported(int inputKey)
{
	if (inputKey < 0 || inputKey > 512)
	{
		std::cout << "testing unsupported input key : " << inputKey << ".\n";
		exit(EXIT_FAILURE);
	}
}

bool is_input_key_up(int inputKey)
{
	is_input_key_supported(inputKey);
	return !inputKeysNow[inputKey];
}

bool is_input_key_down(int inputKey)
{
	is_input_key_supported(inputKey);
	return inputKeysNow[inputKey];
}

bool is_input_key_released(int inputKey)
{
	is_input_key_supported(inputKey);
	return inputKeysBefore[inputKey] && !inputKeysNow[inputKey];
}

bool is_input_key_pressed(int inputKey)
{
	is_input_key_supported(inputKey);
	return !inputKeysBefore[inputKey] && inputKeysNow[inputKey];
}

void update_input_keys()
{
	memcpy(inputKeysBefore, inputKeysNow, sizeof (inputKeysBefore));
}

static GLFWwindow* window = NULL;

void init_window(int width, int height)
{
	// Starting GLFW context
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW.\n";
        exit(EXIT_FAILURE);
    }
    else
    {
        std::cout << "GLFW initialized successfully.\nStarting GLFW context, OpenGL 3.3.\n";
    }

    // Set error callback
    glfwSetErrorCallback(glfwErrorCallback);

    // Some GLFW settings
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 1);

	windowDimensions.x = width;
	windowDimensions.y = height;

    // Create a GLFW window
    window = glfwCreateWindow(windowDimensions.x, windowDimensions.y, "RPG2D", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create GLFW window.\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    else
    {
        std::cout << "GLFW window created successfully.\n";
    }
    glfwMakeContextCurrent(window);

    // Window position
    glfwSetWindowPos(window, 30, 30);

    // Cursor mode
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // Activates V-sync
    glfwSwapInterval(1);

    // Set callback functions
    glfwSetFramebufferSizeCallback(window, glfwResizeCallback);
    glfwSetKeyCallback(window, glfwKeyCallback);
    glfwSetCursorPosCallback(window, glfwMouseCallback);
    glfwSetScrollCallback(window, glfwScrollCallback);

	windowDimensions.x = width;
	windowDimensions.y = height;

	projection = mat4_ortho(0.0f, (float)(windowDimensions.x), (float)(windowDimensions.y), 0.0f);
}

bool windowWillClose = false;
void update_window()
{
	update_delta_time();
	update_input_keys();

	glfwPollEvents();

	windowWillClose = glfwWindowShouldClose(window);
}

void swap_window_buffers()
{
	glfwSwapBuffers(window);
}

void ask_window_to_close()
{
	glfwTerminate();
}

void set_window_name(const char *windowName)
{
	glfwSetWindowTitle(window, windowName);
}

void set_window_vsync(bool sync)
{
	if (sync)
	{
		// Activates V-sync
    glfwSwapInterval(1);
	} else {
		// Deactivates V-sync
    glfwSwapInterval(0);
	}
}

void set_window_clear_color(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
}

void resize_window(int width, int height)
{
	glfwSetWindowSize(window, width, height);
	windowDimensions.x = width;
	windowDimensions.y = height;

	glViewport(0, 0, windowDimensions.x, windowDimensions.y);
	projection = mat4_ortho(0.0f, (float)(windowDimensions.x), (float)(windowDimensions.y), 0.0f);
}