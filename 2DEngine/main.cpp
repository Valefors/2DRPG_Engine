#include <string> //on n'inclue que les include nécessaires pour éviter d'include des fichiers supplémentaires quand on veut accéder qu'au contenu d'un include
#include <vector>

#include <tchar.h>
#include <windows.h>
#include <GLFW/glfw3.h>

#include "log.h"
#include "window.h"
#include "inputs.h"
#include "maths.h"
#include "camera.h"
#include "sprite.h"
#include "graphics.h"
#include "hitbox.h"
#include "physics.h"
#include "map.h"
#include "character.h"

void init()
{
    WindowManager::initWindow(600, 600);
    Graphics::initGraphics();

    Map::init();
    Character::Init((iVec2){500,350}, (iVec2){46,46}, "Assets/character.png", 100);
    Physics::init();

    //Sprite* pokecenter = new Sprite((iVec2){50,30}, (iVec2){224,352}, "Assets/pokecenter.png");
    //Hitbox* hitbox =  new Hitbox((iVec2){50,30}, (iVec2){224,352});

    //Physics::addToHitboxList(hitbox);
}

//argc = argument count (int)
//argv = argument values (char**) nom du programme
//Permet de mettre des paramètres au lancement du programme, modifie le comportement au lancement
int main(int argc, char** argv)
{
    init();
    mciSendString(_T("play Sounds/the-second-step-in-life.wav"), NULL, 0, 0);
    while (true)
    {
        if(!WindowManager::updateWindow())
        {
            WindowManager::askWindowToClose();
            break;
        }

        Character::UpdateCharacter();
        Physics::updatePhysics();
        Camera::updateCamera();
        Graphics::updateGraphics();
        WindowManager::swapWindowBuffers();
    }
    
    return 0;
}