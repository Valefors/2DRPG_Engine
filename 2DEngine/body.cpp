#include <string>

#include "maths.h"
#include "hitbox.h"
#include "sprite.h"
#include "body.h"

Body::Body(iVec2 position, std::string texturePath, iVec2 spriteSize, iVec2 hitboxSize)
{
    _hitbox = new Hitbox(position, hitboxSize);
    _sprite = new Sprite(position, spriteSize, texturePath, false);
}

Body::~Body()
{
    delete _sprite;
    delete _hitbox;
}