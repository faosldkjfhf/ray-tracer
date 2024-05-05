#version 460 core

layout (location = 0) in vec3 position;
layout (location = 2) in vec3 normal;
layout (location = 1) in vec2 texCoord;

out vec2 TexCoord;

void main()
{
    TexCoord = texCoord;
    gl_Position = vec4(position, 1.0);
}
