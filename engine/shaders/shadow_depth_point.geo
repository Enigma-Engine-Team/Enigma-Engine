#version 460 core
layout(triangles, invocations = 6) in;
layout(triangle_strip, max_vertices = 3) out;

layout(std140, binding = 14) uniform LightSpaceMatrices
{
    mat4 lightSpaceMatrices[6];
};

in  vec3 FragPos[];
out vec3 gFragPos;

void main()
{
    for (int i = 0; i < 3; ++i)
    {
        gFragPos    = FragPos[i];
        gl_Position = lightSpaceMatrices[gl_InvocationID] * vec4(FragPos[i], 1.0);
        gl_Layer    = gl_InvocationID;
        EmitVertex();
    }
    EndPrimitive();
}