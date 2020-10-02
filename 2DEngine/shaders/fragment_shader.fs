#version 330 core

in vec2 vertexTexCoord;

out vec4 fragmentColor; //Return variable of the shader

uniform sampler2D tex;

void main()
{
    fragmentColor = texture(tex, vec2(vertexTexCoord.x, vertexTexCoord.y));
    //fragmentColor = vec4(1.0f, 1.0f, 1.0f, 1.0f); //returns fragmentColor, that's why we modify it
}