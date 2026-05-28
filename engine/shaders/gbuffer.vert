#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

layout(binding = 0) uniform CameraData
{
    mat4 projection;
    mat4 view;
    mat4 vp;
    vec3 viewPos;
    int  padding;
} cam;

layout(binding = 2) uniform MeshData
{
    mat4 model;
    mat4 normalMatrix;
} mesh;

layout(location = 0) out vec2 TexCoords;
layout(location = 1) out vec3 WorldPos;
layout(location = 2) out vec3 Normal;

void main()
{
    TexCoords   = aTexCoords;
    WorldPos    = vec3(mesh.model * vec4(aPos, 1.0));
    Normal      = normalize(mat3(mesh.normalMatrix) * aNormal);
    gl_Position = cam.vp * vec4(WorldPos, 1.0);
}
