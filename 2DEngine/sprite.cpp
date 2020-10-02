#include <string>
#include <vector>

#include "maths.h"
#include "sprite.h"
#include "graphics.h"

Sprite::Sprite(iVec2 position, iVec2 size, std::string textureFilePath, bool isBackground) : _position(position), _size(size), _textureID(Graphics::getTexture(textureFilePath)), _isBackground(isBackground)
{
    if(!isBackground) Graphics::addToSortedDrawList(this);
    else Graphics::addToBackgroundDrawList(this); 
}

Sprite::~Sprite()
{
    if(!_isBackground) Graphics::removeFromDrawList(this);
    else Graphics::removeFromBackgroundDrawList(this);
}

void Sprite::SetImage(std::string texturePath)
{
    _textureID = Graphics::getTexture(texturePath);
}

void Sprite::SetPosition(iVec2 position)
{
    _position = position;
}
