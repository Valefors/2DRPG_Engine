#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <cmath>

#include "log.h"
#include "maths.h"
#include "sprite.h"
#include "graphics.h"
#include "hitbox.h"
#include "physics.h"
#include "body.h"
#include "token.h"
#include "map.h"

static char* fileToString(const char* filePath)
{
    FILE* file = fopen(filePath, "rb");
    if (!file)
    {
        DebugLog::WriteLine("Could not open.");
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    if (fileSize == -1L)
    {
        DebugLog::WriteLine("Could not get size.");
    }
    fileSize++; // for '\0'
    rewind(file);

    char *fileString = new char[sizeof (*fileString) * fileSize];
    size_t result = fread(fileString, sizeof (*fileString), fileSize - 1, file);
    if (result != (unsigned long)fileSize - 1)
    {
        DebugLog::WriteLine("Could not read.");
    }
    fileString[fileSize - 1] = '\0';

    fclose(file);

    return fileString;
}

Sprite** Map::_backgroundmap;

void Map::loadTileMap()
{
    std::string file = fileToString("Assets/Maps/horizonCity.map");
    int width = 0;

    while(file[width] != '\r')
    {
        width++;
    }

    int height = 0;

    for(int i = 0; i < file.size(); i++)
    {
       if(file[i] != '\r' && file[i] != '\n')
        {
            height++;
        }
    }

    std::cout << "height before : " << height << '\n';
    height /= width;

    _backgroundmap = new Sprite*[width * height];

    std::cout << "height : " << height << " width : " << width << '\n';

    int tileIndex = 0;

    for(int i = 0; i < file.size(); i++)
    {
        char currentTile = file[i];

        if(currentTile == '\n' || currentTile == '\r') 
        {
            continue;
        }

        if(currentTile == 'W')
        {
            Body* water = new Body((iVec2){34 *(tileIndex % width), 34 * (height - (tileIndex / width))}, "Assets/water.png", (iVec2){34,34}, (iVec2){34,34});
        }

        if(currentTile == 'B')
        {
            Sprite* water = new Sprite((iVec2){34 *(tileIndex % width), 34 * (height - (tileIndex / width))}, (iVec2){34,34}, "Assets/tileground.png", true);
        }

        if(currentTile == 'G')
        {
            Sprite* ground = new Sprite((iVec2){34 *(tileIndex % width), 34 * (height - (tileIndex / width))}, (iVec2){34,34}, "Assets/border.png", true);
        }

        tileIndex++;
    }
}

Token* GetNextToken(std::string& objectMapFile, int& currentCharIndex)
{
    Token* token = new Token();

    while(isspace(objectMapFile[currentCharIndex]))
    {
        currentCharIndex++;
    }

    char currentChar = objectMapFile[currentCharIndex];

    if(currentChar == '\0')
    {
        token->tokenType = E_TokenType::END_OF_FILE;
    }

    else if(isalpha(currentChar))
    {
        token->tokenType = E_TokenType::OBJECT_TYPE;

        while (!isspace(objectMapFile[currentCharIndex]))
        {
            token->stringValue += objectMapFile[currentCharIndex];
            currentCharIndex++;
        }  
    }

    else if(isdigit(currentChar))
    {
        token->tokenType = E_TokenType::NUMERIC;
        char* begin = (char*)objectMapFile.c_str() + currentCharIndex;
        char* end = begin;

        while (!isspace(objectMapFile[currentCharIndex]))
        {
            currentCharIndex++;
            end++;
        }

        token->numericValue = round(strtod(begin, &end));
    }

    else if(currentChar == '"')
    {
        token->tokenType = E_TokenType::TEXTURE_PATH;
        currentCharIndex++;

        while(objectMapFile[currentCharIndex] != '"')
        {
            token->stringValue += objectMapFile[currentCharIndex];
            currentCharIndex++;
        }
        
        currentCharIndex++;
    }
    
    return token;
}

std::vector<Token*> lexObjectMap(std::string& objectMapFile)
{
    std::vector<Token*> tokenArray;
    int currentCharIndex = 0;
    Token* token = GetNextToken(objectMapFile, currentCharIndex);

    while(token->tokenType != E_TokenType::END_OF_FILE)
    {
        tokenArray.push_back(token);
        token = GetNextToken(objectMapFile, currentCharIndex);
    }
    tokenArray.push_back(token);

    return tokenArray;
}

void ParseSprite(std::vector<Token*> tokenArray, int& index)
{
    iVec2 pos = (iVec2){(int)round(tokenArray[index + 1]->numericValue), (int)round(tokenArray[index + 2]->numericValue)};
    iVec2 size = (iVec2){(int)round(tokenArray[index + 3]->numericValue), (int)round(tokenArray[index + 4]->numericValue)};

    std::string texturePath = std::string("Assets/") + tokenArray[index + 5]->stringValue;
    
    if(tokenArray[index + 6]->tokenType != E_TokenType::OBJECT_TYPE && tokenArray[index + 6]->tokenType != E_TokenType::END_OF_FILE)
    {
        DebugLog::WriteLine("Too many parameters in the objectmap for Sprite. Sprite won't be generated.");
        return;
    }

    Sprite* sprite = new Sprite(pos, size, texturePath, false);
}

void ParseBody(std::vector<Token*> tokenArray, int& index)
{
    iVec2 pos = (iVec2){(int)round(tokenArray[index + 1]->numericValue), (int)round(tokenArray[index + 2]->numericValue)};
    iVec2 spriteSize = (iVec2){(int)round(tokenArray[index + 3]->numericValue), (int)round(tokenArray[index + 4]->numericValue)};
    iVec2 hitboxSize = (iVec2){(int)round(tokenArray[index + 5]->numericValue), (int)round(tokenArray[index + 6]->numericValue)};

    std::string texturePath = std::string("Assets/") + tokenArray[index + 7]->stringValue;
    
    std::cout << Token::tokenTypeNames[tokenArray[index + 8]->tokenType] << '\n';
    if(tokenArray[index + 8]->tokenType != E_TokenType::OBJECT_TYPE && tokenArray[index + 8]->tokenType != E_TokenType::END_OF_FILE)
    {
        DebugLog::WriteLine("Too many parameters in the objectmap for Body. Body won't be generated.");
        return;
    }

    Body* sprite = new Body(pos, texturePath, spriteSize, hitboxSize);
}

void parseObjectMap(std::vector<Token*> tokenArray)
{
    for(int i = 0; i < tokenArray.size(); i++)
    {
        if(tokenArray[i]->tokenType == E_TokenType::OBJECT_TYPE)
        {
            if(tokenArray[i]->stringValue == "Body")
            {
                ParseBody(tokenArray, i);
            }

            else if(tokenArray[i]->stringValue == "Sprite")
            {
                ParseSprite(tokenArray, i);
            }
        }
    }
}

std::string Token::tokenTypeNames[] = 
{
    "end of file",
    "end of line",
    "object type",
    "numeric",
    "bb",
    "texture path",
};

void Map::loadObjectMap()
{
    std::string file = fileToString("Assets/Maps/horizonCity.objectmap");
    //std::vector<Token*> tokenArray = lexObjectMap(file);
    parseObjectMap(lexObjectMap(file));
}

void Map::init()
{
    loadTileMap();
    loadObjectMap();
}
