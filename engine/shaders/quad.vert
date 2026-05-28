#version 460 core
layout(location = 0) in vec2 vertex;

uniform vec2 position;
uniform vec2 size;
uniform mat4 projection;

out vec2 uv;

void main()
{
    uv = vertex;
    vec2 worldPos = position + vertex * size;
    gl_Position = projection * vec4(worldPos, 0.0, 1.0);
}