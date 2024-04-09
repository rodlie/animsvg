#version 330 core
layout(location = 0) out vec4 fragColor;
in vec3 pos;

uniform vec3 HSVColor;

uniform float currentValue;
uniform float handleWidth; // fraction of width
uniform bool lightHandle;

vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main(void) {
    float fragValue = 0.5f*(1.f + pos.x);
    if(abs(currentValue - fragValue) < handleWidth) {
        if(lightHandle) {
            fragColor = vec4(1.f, 1.f, 1.f, 1.f);
        } else {
            fragColor = vec4(0.f, 0.f, 0.f, 1.f);
        }
        return;
    }
    vec3 hsvColor = vec3(HSVColor.x, HSVColor.y, fragValue);
    fragColor = vec4(hsv2rgb(hsvColor), 1.f);
}
