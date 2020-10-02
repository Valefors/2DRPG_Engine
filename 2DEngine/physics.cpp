#include <iostream>
#include <string>
#include <vector>

#include "log.h"
#include "maths.h"
#include "sprite.h"
#include "graphics.h"
#include "hitbox.h"
#include "physics.h"
#include "character.h"

std::vector<Hitbox*> Physics::hitboxList;
Hitbox* Physics::_characterHitbox = nullptr;

iVec2 previousCharacterPosition;

void Physics::init()
{
    _characterHitbox = Character::GetCollider();
}

bool Physics::isCollision(Hitbox* h1, Hitbox* h2)
{
    bool downCheck = h1->_position.y > h2->_position.y && h1->_position.y < h2->_position.y + h2->_dimension.y;
    bool upCheck = h1->_position.y + h1->_dimension.y > h2->_position.y && h1->_position.y + h1->_dimension.y < h2->_position.y + h2->_dimension.y;
    bool leftCheck = h1->_position.x > h2->_position.x && h1->_position.x < h2->_position.x + h2->_dimension.x;
    bool rightCheck = h1->_position.x + h1->_dimension.x > h2->_position.x && h1->_position.x + h1->_dimension.x < h2->_position.x + h2->_dimension.x;

    return (downCheck || upCheck) && (leftCheck || rightCheck);
}

void Physics::addToHitboxList(Hitbox* hitbox)
{
    hitboxList.push_back(hitbox);
}

void Physics::removeFromHitboxList(Hitbox* hitbox)
{
    for(std::vector<Hitbox*>::iterator it = hitboxList.begin(); it != hitboxList.end(); ++it)
    {
        if(*it == hitbox)
        {
            hitboxList.erase(it);
            return;
        }
    }
}

void Physics::updatePhysics()
{
    for(int i = 0; i < hitboxList.size(); ++i)
    {
        if(isCollision( _characterHitbox, hitboxList[i]))
        {
            correctCharacterPosition(hitboxList[i]);
        }
    }

    previousCharacterPosition = Character::GetPosition();
}

void Physics::correctCharacterPosition(Hitbox* collidedHitbox)
{
    iVec2 currentPosition = Character::GetPosition();

    // Collision from left
    if (previousCharacterPosition.x + _characterHitbox->_dimension.x <= collidedHitbox->_position.x)
    {
        currentPosition.x = collidedHitbox->_position.x - _characterHitbox->_dimension.x;
    }

    // Collision from right
    if (previousCharacterPosition.x >= collidedHitbox->_position.x + collidedHitbox->_dimension.x)
    {
        currentPosition.x = collidedHitbox->_position.x + collidedHitbox->_dimension.x;
    }

    // Collision from bottom
    if (previousCharacterPosition.y + _characterHitbox->_dimension.y <= collidedHitbox->_position.y)
    {
        currentPosition.y = collidedHitbox->_position.y - _characterHitbox->_dimension.y;
    }

    // Collision from top
    if (previousCharacterPosition.y >= collidedHitbox->_position.y + collidedHitbox->_dimension.y)
    {
        currentPosition.y = collidedHitbox->_position.y + collidedHitbox->_dimension.y;
    }

    Character::SetPosition(currentPosition);
}