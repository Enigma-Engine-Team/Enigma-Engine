#version 460 core

layout(location = 0) out vec4 FragColor;
layout(location = 0) in vec3 WorldPos;

layout(binding = 3) uniform sampler2D equirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 SampleSphericalMap(vec3 v)
{
    vec3 dir = normalize(v);
    
    vec2 uv = vec2(atan(dir.z, dir.x), asin(clamp(dir.y, -1.0, 1.0)));
    
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{        
    vec2 uv = SampleSphericalMap(WorldPos);
    vec3 color = texture(equirectangularMap, uv).rgb;
    
    if (isnan(color.r) || isnan(color.g) || isnan(color.b) || 
        isinf(color.r) || isinf(color.g) || isinf(color.b))
    {
        color = vec3(0.0);
    }

    color = clamp(color, vec3(0.0), vec3(1000.0));

    FragColor = vec4(color, 1.0);
}