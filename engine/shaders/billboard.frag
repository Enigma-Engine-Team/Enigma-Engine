#version 460

out vec4 FragColor;
in vec3 WorldPos;
in vec2 TexCoords;

layout (binding = 1) uniform sampler2D textureIcon;
uniform float opacity;

void main()
{
    vec4 color = texture(textureIcon, TexCoords);

    color.a *= opacity;

    if(color.a < 0.1)
        discard;

    FragColor = color;
}