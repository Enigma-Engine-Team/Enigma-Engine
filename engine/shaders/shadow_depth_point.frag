#version 460 core

in  vec3 gFragPos;

uniform vec3  lightPos;
uniform float farPlane;

void main()
{
    float lightDist = length(gFragPos - lightPos);
    gl_FragDepth    = lightDist / farPlane;
}