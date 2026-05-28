#version 460 core

layout (location = 0) in vec3 aPos;

layout (binding = 0) uniform CameraData
{
    mat4 projection;
    mat4 view;
    mat4 vp;
    vec3 viewPos;
    int padding;
}cam;

layout (location = 0) out vec3 WorldPos;

void main()
{
    WorldPos = aPos;

	mat4 rotView = mat4(mat3(cam.view));
	vec4 clipPos = cam.projection * rotView * vec4(WorldPos, 1.0);

	gl_Position = clipPos.xyww;
}