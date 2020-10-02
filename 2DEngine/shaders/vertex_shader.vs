#version 330 core

layout (location = 0) in vec2 layoutPosition;  //Take what we have in the channel 0 of the VBO and put it in the layoutPosition
layout (location = 1) in vec2 layoutTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 vertexTexCoord;

void main()
{
    //A vertex shader always returns a gl_Position, that's why we modify it
    gl_Position = projection * view * model * vec4(layoutPosition, 0.0f, 1.0f); //Layout position : x & y, z, w
    vertexTexCoord = layoutTexCoord;
}
