#include <string>
#include <vector>
#include <algorithm>

#include <GL/gl3w.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "log.h"
#include "maths.h"
#include "camera.h"
#include "sprite.h"
#include "graphics.h"
#include "hitbox.h"
#include "character.h"

unsigned int VAO_ID; //Vertex Array Object. Store datas needed to read a VBO_ID correctly
unsigned int VBO_ID; //Vertex Buffer Object. Store datas to draw an object (position, color, texture position)
unsigned int EBO_ID; //Element Buffer Object. Store vertices index to draw an object to avoid vertex duplication

unsigned int shaderProgram_ID;
//unsigned int texture_ID;

float vertices[16] =
{
    // Pos      //Tex
    0.0f, 1.0f, 0.0f, 1.0f, 
    1.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 1.0f, 1.0f
};

//EBO array. One line per triangle, elements are vertex indices
unsigned int elements[6] =
{
    2, 0, 3,
    2, 3, 1
};

//Need a definition of the private var to use it. Implicit call to default constructor of vector
std::vector<Sprite*> Graphics::spriteToDraw;
std::vector<Sprite*> Graphics::backgroundToDraw;

char* fileToString(const char* filePath)
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

unsigned int linkShader(unsigned vertexShaderId, unsigned fragmentShaderId)
{
// Link shader program
    unsigned int programId = glCreateProgram();
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);
    glLinkProgram(programId); //Link fs and vs, in and out

    // Print linking errors
    int success;
    char infoLog[512];
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(programId, 512, NULL, infoLog);
        DebugLog::WriteLine("Failed to link shader program.");
        exit(EXIT_FAILURE);
    }

    return programId;
}

unsigned int compileShader(std::string filePath, GLenum shaderType)
{
// Read file into char array
    const char *codeChar = fileToString(filePath.c_str());

    // Generate and compile shader
    unsigned int shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &codeChar, NULL);
    glCompileShader(shader);

    free((void*)codeChar);

    // Print compile errors
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success); //Using of a & to send a ptr to write in the value
    if(!success)
    {
    char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        DebugLog::WriteLine("Shader compilation failed.");
        exit (EXIT_FAILURE);
    }

    return shader;
}

unsigned int Graphics::getTexture(std::string texturePath)
{
    unsigned int textureID;

    glGenTextures(1, &textureID); //We want one Texture_ID. Reserve a Texture_ID in the VRAM and return the Texture_ID to communicate between the program and the VRAM with OpenGL
    glBindTexture(GL_TEXTURE_2D, textureID);

    //Functions to modify drawing parameters of the texture (here we want pixel effect)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //Load texture
    int width, height, nbChannels;

    //Char* is an array of char bc we don't know the complete size of the array and the texture
    //We want to openGL to write in our values the width, height. As we want RGBA, we want texel with 4 channels only
    unsigned char* texels = stbi_load(texturePath.c_str(), &width, &height, &nbChannels, 4);

    if(texels == nullptr) 
    {
        DebugLog::WriteLine("Texture not loaded");
        exit(EXIT_FAILURE);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texels);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(texels);

    return textureID;
}

int getUniformLocationInShader(unsigned int shaderProgramID, std::string uniformVarName)
{
    int location = glGetUniformLocation(shaderProgramID, uniformVarName.c_str());

    if(location == -1)
    {
        DebugLog::WriteLine("Could not get uniform variable location in shader.");
        exit(EXIT_FAILURE);
    }

    return location;
}

void sendIntToShader(unsigned int shaderProgramID, std::string uniformVarName, int value)
{
    //We send 1 int to uniform
    glUniform1i(getUniformLocationInShader(shaderProgramID, uniformVarName), value);
}

void sendMat4ToShader(unsigned int shaderProgramID, std::string uniformVarName, mat4 mat4Value)
{
    glUniformMatrix4fv(getUniformLocationInShader(shaderProgramID, uniformVarName), 1, GL_FALSE, &mat4Value.e00);
}

//Initialise le module graphique du programme
void Graphics::initGraphics()
{
    if(gl3wInit()) //return an int id error. If return 0 (false) = no error
    {
        DebugLog::WriteLine("Failed to initialize GL3W.");
        exit(EXIT_FAILURE);
    }

    if(!gl3wIsSupported(3, 3)) //Check if the graphic card supports OpenGL 3.3
    {
        DebugLog::WriteLine("Ur graphic card sucks.");
        exit(EXIT_FAILURE);
    }

    glEnable(GL_BLEND);                                 //Enable transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  //Use the texture's alpha channel to make transparency

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);     //Indication for OpenGL : a triangle front face is clock wise indexed
    glCullFace(GL_FRONT);    //The back of triangle face isn't drawn
    
    glClearColor(0,0,0,1);  //Fix the background color
    
    glGenVertexArrays(1, &VAO_ID); //We want one VAO_ID. Reserve a VAO_ID in the VRAM and return the VAO_ID to communicate between the program and the VRAM with OpenGL
    glGenBuffers(1, &VBO_ID); //Same VAO_ID logic
    glGenBuffers(1, &EBO_ID);

    glBindVertexArray(VAO_ID); //OpenGL state machine. One step at time. We indicate what VAO do we use for the next manipulations.
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO_ID); //Bind the VBO with the VAO. This VAO contains datas to read the VBO. We indicate what VBO we use for the next manipulations.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 
    //GL_STATIC_DRAW : the shape will rarely be modified. The datas will be put in a far away part of the VRAM because we don't need to access it often
    //Fill VBO in buffer of vertices array

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_ID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); //Announces to OpenGL that VAO will store datas to read channel 0 of VBO
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); //Channel 0 of VBO contains 2 float variables and a VBO vertex has a size of 4 float. Channel 0 begins at adress 0 of the VBO

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    //We need a program to draw. This program need the vertex shader and the fragment shader
    shaderProgram_ID = linkShader(compileShader("shaders/vertex_shader.vs", GL_VERTEX_SHADER), compileShader("shaders/fragment_shader.fs", GL_FRAGMENT_SHADER));

    //texture_ID = getTexture("Assets/missile.png");
}

void updateDrawList()
{   
    Sprite* characterSprite = Character::GetSprite();
    Graphics::removeFromDrawList(characterSprite);
    Graphics::addToSortedDrawList(characterSprite);
}

void Graphics::updateGraphics()
{
    glClear(GL_COLOR_BUFFER_BIT);   //We clear only colors
    glActiveTexture(GL_TEXTURE0);   //Activate texture zone in the graphic card
    glBindVertexArray(VAO_ID);      // Bind vao
    glUseProgram(shaderProgram_ID); //Indicate to OpenGL what program to use if we want to draw something

    mat4 view; //Camera matrix
    iVec2 position = Camera::getPosition();
    view.Translate((iVec2){300 -position.x, -300 - position.y});

    updateDrawList();

    for(int i = 0; i < backgroundToDraw.size(); i++)
    {
        Sprite* sprite = backgroundToDraw[i];
        glBindTexture(GL_TEXTURE_2D, sprite->_textureID);

        mat4 model; // Identity matrix
        model.Translate((iVec2){sprite->_position.x, 600 - sprite->_position.y - sprite->_size.y});
        model.Scale(sprite->_size);

        sendIntToShader(shaderProgram_ID, "tex", 0);
        sendMat4ToShader(shaderProgram_ID, "projection", mat4::GetOrthogonalProjectionMat4(600, 600));
        sendMat4ToShader(shaderProgram_ID, "view", view);
        sendMat4ToShader(shaderProgram_ID, "model", model);

        // Draw 3D object in vbo
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    for(int i = 0; i < spriteToDraw.size(); i++)
    {
        Sprite* sprite = spriteToDraw[i];
        glBindTexture(GL_TEXTURE_2D, sprite->_textureID);

        mat4 model; // Identity matrix
        model.Translate((iVec2){sprite->_position.x, 600 - sprite->_position.y - sprite->_size.y});
        model.Scale(sprite->_size);

        sendIntToShader(shaderProgram_ID, "tex", 0);
        sendMat4ToShader(shaderProgram_ID, "projection", mat4::GetOrthogonalProjectionMat4(600, 600));
        sendMat4ToShader(shaderProgram_ID, "view", view);
        sendMat4ToShader(shaderProgram_ID, "model", model);

        // Draw 3D object in vbo
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
}

void Graphics::addToBackgroundDrawList(Sprite* sprite)
{
    backgroundToDraw.push_back(sprite);
}

void Graphics::addToSortedDrawList(Sprite* sprite)
{
    for(int i = 0; i < spriteToDraw.size(); i++)
    {
        if(sprite->_position.y > spriteToDraw[i]->_position.y)
        {
            spriteToDraw.insert(spriteToDraw.begin() + i, sprite);
            return;
        }
    }

    spriteToDraw.push_back(sprite); //If sprite is behind everything on the array
}

void Graphics::removeFromBackgroundDrawList(Sprite* sprite)
{
    backgroundToDraw.erase(std::remove(backgroundToDraw.begin(), backgroundToDraw.end(), sprite), backgroundToDraw.end());
}

void Graphics::removeFromDrawList(Sprite* sprite)
{
    spriteToDraw.erase(std::remove(spriteToDraw.begin(), spriteToDraw.end(), sprite), spriteToDraw.end());
}

