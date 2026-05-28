#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 WorldPos;

layout ( binding = 0) uniform GizmoData
{
    mat4 model;
    mat4 vp;
    vec4 color;
} gizmo;

void main() 
{
    TexCoords = aTexCoords;
    WorldPos = aPos;

    vec3 camRight = vec3(gizmo.vp[0][0], gizmo.vp[1][0], gizmo.vp[2][0]);
    vec3 camUp = normalize(vec3(gizmo.vp[0][1], gizmo.vp[1][1], gizmo.vp[2][1]));
    vec3 billboardPos = aPos.x * camRight + aPos.y * camUp;

    gl_Position = gizmo.vp * gizmo.model * vec4(billboardPos, 1.0);
}