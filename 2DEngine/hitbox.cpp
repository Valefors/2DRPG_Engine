#include <vector>

#include "maths.h"
#include "hitbox.h"
#include "physics.h"

Hitbox::Hitbox(iVec2 position, iVec2 size) : _position(position), _dimension(size)
{
    Physics::addToHitboxList(this);
}

Hitbox::~Hitbox()
{
    Physics::removeFromHitboxList(this);
}

void Hitbox::SetPosition(iVec2 position)
{
    _position = position;
}