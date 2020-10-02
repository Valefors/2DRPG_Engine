#include <cstring> //String class from C language
#include <string>
#include <GLFW/glfw3.h>

#include "log.h"
#include "inputs.h"
#include "window.h"

static GLFWwindow* window = nullptr;

//Static file var to be accessed by both keyCallback function and InputsManager without aving to include GLFW when using InputsManager
static bool inputsKeysBefore[KeyCode::KEYCODE_COUNT] = {};
static bool inputsKeysNow[KeyCode::KEYCODE_COUNT] = {};

//KeyCallback isn't a function of the WindowManager class to avoid to include GLFW.h when we include window.h
static void keyCallback(GLFWwindow* window, int key, int scanCode, int action, int mode)
{
    if(action == GLFW_PRESS)
    {
        inputsKeysNow[key] =  true; //key is the same value as the wanted keycode in the enum
    }

    else if(action == GLFW_RELEASE)
    {
        inputsKeysNow[key] =  false;
    }
}

bool InputsManager::getKey(KeyCode keyCode)
{
    return inputsKeysNow[keyCode];
}

double WindowManager::previousTime = 0;
double WindowManager::deltaTime = 0;

void WindowManager::initWindow(int pWidth, int pHeight)
{
    if(!glfwInit()) 
    {
        DebugLog::WriteLine("Failed window initialization."); //Convert char* to String implicitly
        exit(EXIT_FAILURE);
    }

    //Paramètres pour la fenêtre
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 1);

    window = glfwCreateWindow(pWidth, pHeight, "2D ENGINE", nullptr, nullptr);
    
    if(window == nullptr) 
    {
        DebugLog::WriteLine("Failed window initialization.");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    //Création du contexte de dessin
    glfwMakeContextCurrent(window);

    //Callback pour les inputs
    glfwSetKeyCallback(window, keyCallback);

    //Initialisation du deltaTime
    previousTime = glfwGetTime();
}

bool WindowManager::updateWindow()
{
    double currentTime = glfwGetTime();

    deltaTime = currentTime - previousTime;
    previousTime = currentTime;

    //Copy last inputs states into buffer. Before glfwPollEvents to get previous inputs
    updateInputsKeys(); 

    //Get all inputs from Windows
    glfwPollEvents();

    return !glfwWindowShouldClose(window);
}

void WindowManager::swapWindowBuffers()
{
    glfwSwapBuffers(window); //Sent datas to draw for screen and graphic card buffers
}

void WindowManager::updateInputsKeys()
{
    memcpy(inputsKeysBefore, inputsKeysNow, sizeof(inputsKeysBefore)); //C function. Copy a part from the ram to an other place. Avoid loops
}

void WindowManager::askWindowToClose()
{
    glfwTerminate();
}