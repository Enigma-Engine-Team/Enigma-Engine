#version 460 core

layout (location = 0) in vec3 aPos;

layout (binding = 0) uniform GizmoData
{
    mat4 model;
    mat4 vp;
    vec4 color;
} gizmo;

void main() 
{
    gl_Position = gizmo.vp * gizmo.model * vec4(aPos, 1.0);
}
