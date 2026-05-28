#version 460 core

layout(location = 0) out vec3 WorldPos;

uniform float gridSize = 500.0;

layout(binding = 0) uniform CameraData 
{
    mat4 projection;
    mat4 view;
    mat4 vp;
    vec3 viewPos;
    int padding;
} cam;

const vec3 gridPos[4] = vec3[4](
	vec3(-1.0, 0.0, -1.0),
	vec3( 1.0, 0.0, -1.0),
	vec3( 1.0, 0.0,  1.0),
	vec3(-1.0, 0.0,  1.0)
);

const int indices[6] = int[6](
	0, 2, 1,
	2, 0, 3
);

void main() 
{
	int index = indices[gl_VertexID];

	vec3 Pos = gridPos[index] * gridSize;

	Pos.x += cam.viewPos.x;
	Pos.z += cam.viewPos.z;

	vec4 Pos4 = vec4(Pos, 1.0);

	gl_Position = cam.vp * Pos4;

	WorldPos = Pos;
}