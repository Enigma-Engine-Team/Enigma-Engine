#version 460 core
out vec4 color;
in vec2 uv;

uniform sampler2D bgTexture;
uniform bool useTexture;
uniform vec4 bgColor;
uniform vec2 size;
uniform float radius;

float RoundedBoxSDF(vec2 pixelPos, vec2 halfSize, float r)
{
    vec2 q = abs(pixelPos - halfSize) - halfSize + r;
    return length(max(q, 0.0)) - r;
}

void main()
{
    vec2 pixelPos = uv * size;
    float dist  = RoundedBoxSDF(pixelPos, size * 0.5, radius);
    float alpha = 1.0 - smoothstep(-1.0, 1.0, dist);

    vec4 finalColor;
    if (useTexture)
        finalColor = texture(bgTexture, uv);
    else
        finalColor = bgColor;

    color = vec4(finalColor.rgb, finalColor.a * alpha);
}