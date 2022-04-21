#version 460

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TextureCoordinates;

out vec2 v_TextureCoordinates;

void main()
{
    gl_Position = a_Position;
    v_TextureCoordinates = a_TextureCoordinates;
}
