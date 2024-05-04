#version 460 core

in vec2 TexCoord;

uniform sampler2D u_Texture;
uniform vec2 u_Resolution;

out vec4 fragColor;

void main()
{
    fragColor = texture(u_Texture, gl_FragCoord.xy / u_Resolution);
}
