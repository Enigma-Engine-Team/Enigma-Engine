#version 460 core

layout(location = 0) in vec3 WorldPos;
layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform CameraData 
{
    mat4 projection;
    mat4 view;
    mat4 vp;
    vec3 viewPos;
    int padding;
} cam;

const float gridMinPixelsBetweenCells = 2.0;
const float gridCellSize = 1.0;
const vec4 gridColor = vec4(0.8, 0.8, 0.8, 0.6);
const vec4 gridColorAlt = vec4(0.5, 0.5, 0.5, 0.7);
const float maxDist = 500.0;

float log10(float x)
{
	return log(x) / log(10.0);
}

void main()
{
	vec2 dvx = vec2(dFdx(WorldPos.x), dFdy(WorldPos.x));
	vec2 dvy = vec2(dFdx(WorldPos.z), dFdy(WorldPos.z));

	float lx = length(dvx);
	float ly = length(dvy);

	vec2 dudv = vec2(lx, ly);

	float l = length(dudv);

	float LOD = max(0.0, log10(l * gridMinPixelsBetweenCells / gridCellSize) + 1.0);

	float gridCellSizeLOD = gridCellSize * pow(10.0, floor(LOD));
	float gridCellSizeLOD1 = gridCellSizeLOD * 10.0;
	float gridCellSizeLOD2 = gridCellSizeLOD1 * 10.0;

	dudv *= 4.0;
	
	float tx = mod(WorldPos.x, gridCellSizeLOD) / dudv.x;
	float ty = mod(WorldPos.z, gridCellSizeLOD) / dudv.y;

	float Lod0a = max(1.0 - abs(clamp(tx, 0.0, 1.0) * 2.0 - 1.0),
                  1.0 - abs(clamp(ty, 0.0, 1.0) * 2.0 - 1.0));

	tx = mod(WorldPos.x, gridCellSizeLOD1) / dudv.x;
	ty = mod(WorldPos.z, gridCellSizeLOD1) / dudv.y;

	float Lod0a1 = max(1.0 - abs(clamp(tx, 0.0, 1.0) * 2.0 - 1.0),
                  1.0 - abs(clamp(ty, 0.0, 1.0) * 2.0 - 1.0));

	tx = mod(WorldPos.x, gridCellSizeLOD2) / dudv.x;
	ty = mod(WorldPos.z, gridCellSizeLOD2) / dudv.y;

	float Lod0a2 = max(1.0 - abs(clamp(tx, 0.0, 1.0) * 2.0 - 1.0),
                  1.0 - abs(clamp(ty, 0.0, 1.0) * 2.0 - 1.0));

	float LodFade = fract(LOD);

	vec4 color;

	if(Lod0a2 > 0.0)
	{
		color = gridColor;
		color.a *= Lod0a2;
	}
	else 
	{
		if(Lod0a1 > 0.0)
		{
			color = mix(gridColor, gridColorAlt, LodFade);
			color.a *= Lod0a1;
		}
		else
		{
			color = gridColorAlt;
			color.a *= (Lod0a * (1.0 - LodFade));
		}
	}

	float dist = length(WorldPos.xz - cam.viewPos.xz);
	float fallOffAlpha = clamp(1.0 - dist / maxDist, 0.0, 1.0);

	color.a *= fallOffAlpha;

	const float gamma = 2.2;

	color = pow(color, vec4(gamma));

	color = -log(vec4(1.0) - color);

	FragColor = color;
}