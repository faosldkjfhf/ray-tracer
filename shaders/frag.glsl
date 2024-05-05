#version 460 core

in vec2 TexCoord;

uniform sampler2D u_Texture;

out vec4 fragColor;

void main()
{
    vec3 texColor = texture(u_Texture, TexCoord).rgb;
    fragColor = vec4(texColor, 1.0f);
}
