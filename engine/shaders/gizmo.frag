#version 460 core

out vec4 FragColor;

layout ( binding = 0) uniform GizmoData
{
    mat4 model;
    mat4 vp;
    vec4 color;
}gizmo;

void main() 
{
    FragColor = gizmo.color;
}