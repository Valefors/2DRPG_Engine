#include <string>
#include <vector>
#include <cmath>

#include "maths.h"
#include "globals.h"
#include "sprite.h"
#include "graphics.h"
#include "hitbox.h"
#include "physics.h"
#include "inputs.h"
#include "character.h"

iVec2 Character::_position = iVec2{0,0};
Sprite* Character::_sprite = nullptr;
Hitbox* Character::_hitbox = nullptr;
int Character::_characterSpeed = 0;

void Character::Init(iVec2 position, iVec2 size, std::string texture, int speed)
{
    _position = position;
    _sprite = new Sprite(_position, size, texture, false);
    _hitbox = new Hitbox(_position, iVec2{size.x, size.y / 2});
    _characterSpeed = speed;
}

void Character::UpdateCharacter()
{   
    if(InputsManager::getKey(KeyCode::RIGHT_ARROW))
    {
        //std::cout << GlobalDatas::getDeltaTime() << std::endl;
        _sprite->SetImage("Assets/Character/right_idle.png");
        _position.x += round(_characterSpeed * GlobalDatas::getDeltaTime());
    }

    if(InputsManager::getKey(KeyCode::LEFT_ARROW))
    {
        _sprite->SetImage("Assets/Character/left_idle.png");
        _position.x -= round(_characterSpeed * GlobalDatas::getDeltaTime());
    }

    if(InputsManager::getKey(KeyCode::UP_ARROW))
    {
        _sprite->SetImage("Assets/Character/back_idle.png");
        _position.y += round(_characterSpeed * GlobalDatas::getDeltaTime());
    }

    if(InputsManager::getKey(KeyCode::DOWN_ARROW))
    {
        _sprite->SetImage("Assets/Character/face_idle.png");
        _position.y -= round(_characterSpeed * GlobalDatas::getDeltaTime());
    }

    _sprite->SetPosition(_position);
    _hitbox->SetPosition(_position);
}

iVec2 Character::GetPosition()
{
    return _position;
}

void Character::SetPosition(iVec2 pos)
{
    _position = pos;
    _sprite->SetPosition(_position);
    _hitbox->SetPosition(_position);
}

Sprite* Character::GetSprite()
{
    return _sprite;
}

Hitbox* Character::GetCollider()
{
    return _hitbox;
}