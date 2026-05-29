#version 460 core
out float FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform SSAOHeader
{
    vec4 samples[64];

    mat4 projection;
    mat4 view;

    float radius;
    float bias;
    int kernelSize;
    float width;

    float height;
    float padding[3];

} ssaoHeader;

layout (binding = 1) uniform sampler2D gPosition;
layout (binding = 2) uniform sampler2D gNormal;
layout (binding = 3) uniform sampler2D texNoise;


void main()
{
    vec3 fragPos = (ssaoHeader.view * vec4(texture(gPosition, TexCoords).xyz, 1.0)).xyz;
    vec3 normal  = normalize(mat3(transpose(inverse(ssaoHeader.view))) * texture(gNormal, TexCoords).rgb);

    vec2 noiseScale = vec2(ssaoHeader.width / 4.0, ssaoHeader.height / 4.0);
    vec3 randomVec  = normalize(texture(texNoise, TexCoords * noiseScale).xyz);

    vec3 tangent   = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN       = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for(int i = 0; i < ssaoHeader.kernelSize; ++i)
    {
        vec3 samplePos = TBN * ssaoHeader.samples[i].xyz;
        samplePos = fragPos + samplePos * ssaoHeader.radius;

        vec4 offset = ssaoHeader.projection * vec4(samplePos, 1.0);
        offset.xyz /= offset.w;
        offset.xyz  = offset.xyz * 0.5 + 0.5;

        // Même transformation pour le depth lu depuis le GBuffer
        float sampleDepth = (ssaoHeader.view * vec4(texture(gPosition, offset.xy).xyz, 1.0)).z;

        float rangeCheck = smoothstep(0.0, 1.0, ssaoHeader.radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + ssaoHeader.bias ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = 1.0 - (occlusion / float(ssaoHeader.kernelSize));
    occlusion = pow(occlusion, 3.0);
    FragColor = occlusion;
}
