#version 330 core
layout(location = 0) out vec4 fragColor;
in vec3 pos;

uniform vec3 RGBColor;

uniform float currentValue;
uniform float handleWidth; // fraction of width
uniform bool lightHandle;
uniform vec2 meshSize;

vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main(void) {
    float posFrac = 0.5f*(1.f + pos.x);
    float fragAlpha = posFrac;
    if(abs(currentValue - fragAlpha) < handleWidth) {
        if(lightHandle) {
            fragColor = vec4(1.f, 1.f, 1.f, 1.f);
        } else {
            fragColor = vec4(0.f, 0.f, 0.f, 1.f);
        }
        return;
    }
    vec3 meshColor;
    int hId = int(floor(posFrac/meshSize.x));
    int vId = int(floor(0.5f*(1.f + pos.y)/meshSize.y));
    if((hId + vId) % 2 == 0) {
        meshColor = vec3(0.2f, 0.2f, 0.2f);
    } else {
        meshColor = vec3(0.4f, 0.4f, 0.4f);
    }
    fragColor = vec4(mix(meshColor, RGBColor, fragAlpha), 1.f);
}
