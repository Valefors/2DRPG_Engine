#include <string>
#include <iostream>
#include <cmath>

#include "maths.h"
#include "inputs.h"
#include "globals.h"
#include "log.h"
#include "sprite.h"
#include "hitbox.h"
#include "character.h"
#include "camera.h"

iVec2 Camera::_position{0,0}; //As a static var, needs to be defined outside class
int Camera::_cameraSpeed = 50;

const iVec2& Camera::getPosition()
{
    return _position;
}

void Camera::updateCamera()
{
    //Free Camera
    /*if(InputsManager::getKey(KeyCode::RIGHT_ARROW))
    {
        _position.x += round(_cameraSpeed * GlobalDatas::getDeltaTime());
    }

    if(InputsManager::getKey(KeyCode::LEFT_ARROW))
    {
        _position.x -= round(_cameraSpeed * GlobalDatas::getDeltaTime());
    }

    if(InputsManager::getKey(KeyCode::UP_ARROW))
    {
        _position.y -= round(_cameraSpeed * GlobalDatas::getDeltaTime());
    }

    if(InputsManager::getKey(KeyCode::DOWN_ARROW))
    {
        _position.y += round(_cameraSpeed * GlobalDatas::getDeltaTime());
    }*/

    _position = (iVec2){Character::GetPosition().x, -Character::GetPosition().y};
}