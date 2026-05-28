#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform vec2 texelStep; // = vec2(1.0/width, 1.0/height)

const float EDGE_THRESHOLD_MIN = 0.0312;
const float EDGE_THRESHOLD_MAX = 0.125;
const float QUALITY[12] = float[](1.0,1.0,1.0,1.0,1.0,1.5,2.0,2.0,2.0,2.0,4.0,8.0);
const int   ITERATIONS   = 12;
const float SUBPIXEL_QUALITY = 0.75;

float rgb2luma(vec3 rgb) {
    return dot(rgb, vec3(0.299, 0.587, 0.114));
}

void main()
{
    vec3 colorCenter = texture(screenTexture, TexCoords).rgb;

    float lumaCenter = rgb2luma(colorCenter);
    float lumaDown   = rgb2luma(texture(screenTexture, TexCoords + vec2( 0.0, -texelStep.y)).rgb);
    float lumaUp     = rgb2luma(texture(screenTexture, TexCoords + vec2( 0.0,  texelStep.y)).rgb);
    float lumaLeft   = rgb2luma(texture(screenTexture, TexCoords + vec2(-texelStep.x,  0.0)).rgb);
    float lumaRight  = rgb2luma(texture(screenTexture, TexCoords + vec2( texelStep.x,  0.0)).rgb);

    float lumaMin = min(lumaCenter, min(min(lumaDown, lumaUp), min(lumaLeft, lumaRight)));
    float lumaMax = max(lumaCenter, max(max(lumaDown, lumaUp), max(lumaLeft, lumaRight)));
    float lumaRange = lumaMax - lumaMin;

    if (lumaRange < max(EDGE_THRESHOLD_MIN, lumaMax * EDGE_THRESHOLD_MAX)) {
        FragColor = vec4(colorCenter, 1.0);
        return;
    }

    // Coins
    float lumaDownLeft  = rgb2luma(texture(screenTexture, TexCoords + vec2(-texelStep.x, -texelStep.y)).rgb);
    float lumaUpRight   = rgb2luma(texture(screenTexture, TexCoords + vec2( texelStep.x,  texelStep.y)).rgb);
    float lumaUpLeft    = rgb2luma(texture(screenTexture, TexCoords + vec2(-texelStep.x,  texelStep.y)).rgb);
    float lumaDownRight = rgb2luma(texture(screenTexture, TexCoords + vec2( texelStep.x, -texelStep.y)).rgb);

    float lumaDownUp    = lumaDown + lumaUp;
    float lumaLeftRight = lumaLeft + lumaRight;
    float lumaLeftCorners  = lumaDownLeft  + lumaUpLeft;
    float lumaDownCorners  = lumaDownLeft  + lumaDownRight;
    float lumaRightCorners = lumaDownRight + lumaUpRight;
    float lumaUpCorners    = lumaUpRight   + lumaUpLeft;

    float edgeHorizontal = abs(-2.0*lumaLeft   + lumaLeftCorners)
                         + abs(-2.0*lumaCenter  + lumaDownUp) * 2.0
                         + abs(-2.0*lumaRight   + lumaRightCorners);
    float edgeVertical   = abs(-2.0*lumaUp      + lumaUpCorners)
                         + abs(-2.0*lumaCenter  + lumaLeftRight) * 2.0
                         + abs(-2.0*lumaDown    + lumaDownCorners);

    bool isHorizontal = (edgeHorizontal >= edgeVertical);

    float luma1 = isHorizontal ? lumaDown : lumaLeft;
    float luma2 = isHorizontal ? lumaUp   : lumaRight;
    float gradient1 = luma1 - lumaCenter;
    float gradient2 = luma2 - lumaCenter;
    bool is1Steepest = abs(gradient1) >= abs(gradient2);
    float gradientScaled = 0.25 * max(abs(gradient1), abs(gradient2));

    float stepLength = isHorizontal ? texelStep.y : texelStep.x;
    float lumaLocalAverage;
    if (is1Steepest) {
        stepLength = -stepLength;
        lumaLocalAverage = 0.5 * (luma1 + lumaCenter);
    } else {
        lumaLocalAverage = 0.5 * (luma2 + lumaCenter);
    }

    vec2 currentUV = TexCoords;
    if (isHorizontal) currentUV.y += stepLength * 0.5;
    else              currentUV.x += stepLength * 0.5;

    vec2 offset = isHorizontal ? vec2(texelStep.x, 0.0) : vec2(0.0, texelStep.y);
    vec2 uv1 = currentUV - offset;
    vec2 uv2 = currentUV + offset;

    float lumaEnd1 = rgb2luma(texture(screenTexture, uv1).rgb) - lumaLocalAverage;
    float lumaEnd2 = rgb2luma(texture(screenTexture, uv2).rgb) - lumaLocalAverage;
    bool reached1  = abs(lumaEnd1) >= gradientScaled;
    bool reached2  = abs(lumaEnd2) >= gradientScaled;

    if (!reached1) uv1 -= offset;
    if (!reached2) uv2 += offset;

    if (!reached1 || !reached2) {
        for (int i = 2; i < ITERATIONS; i++) {
            if (!reached1) {
                lumaEnd1 = rgb2luma(texture(screenTexture, uv1).rgb) - lumaLocalAverage;
                uv1 -= offset * QUALITY[i];
            }
            if (!reached2) {
                lumaEnd2 = rgb2luma(texture(screenTexture, uv2).rgb) - lumaLocalAverage;
                uv2 += offset * QUALITY[i];
            }
            reached1 = abs(lumaEnd1) >= gradientScaled;
            reached2 = abs(lumaEnd2) >= gradientScaled;
            if (reached1 && reached2) break;
        }
    }

    float distance1 = isHorizontal ? (TexCoords.x - uv1.x) : (TexCoords.y - uv1.y);
    float distance2 = isHorizontal ? (uv2.x - TexCoords.x) : (uv2.y - TexCoords.y);
    bool isDirection1 = distance1 < distance2;
    float distanceFinal = min(distance1, distance2);
    float edgeThickness = distance1 + distance2;

    float pixelOffset = -distanceFinal / edgeThickness + 0.5;

    bool isLumaCenterSmaller = lumaCenter < lumaLocalAverage;
    bool correctVariation    = ((isDirection1 ? lumaEnd1 : lumaEnd2) < 0.0) != isLumaCenterSmaller;
    float finalOffset = correctVariation ? pixelOffset : 0.0;

    float lumaAverage = (1.0/12.0) * (2.0*(lumaDownUp + lumaLeftRight) + lumaLeftCorners + lumaRightCorners);
    float subPixelOffset1 = clamp(abs(lumaAverage - lumaCenter) / lumaRange, 0.0, 1.0);
    float subPixelOffset2 = (-2.0 * subPixelOffset1 + 3.0) * subPixelOffset1 * subPixelOffset1;
    float subPixelOffsetFinal = subPixelOffset2 * subPixelOffset2 * SUBPIXEL_QUALITY;
    finalOffset = max(finalOffset, subPixelOffsetFinal);

    vec2 finalUV = TexCoords;

    if (isHorizontal) 
        finalUV.y += finalOffset * stepLength;
    else              
        finalUV.x += finalOffset * stepLength;

    FragColor = vec4(texture(screenTexture, finalUV).rgb, 1.0);
}