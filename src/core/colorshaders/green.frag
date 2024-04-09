#version 330 core
layout(location = 0) out vec4 fragColor;
in vec3 pos;

uniform vec3 RGBColor;

uniform float currentValue;
uniform float handleWidth; // fraction of width
uniform bool lightHandle;

void main(void) {
    float fragGreen = 0.5f*(1.f + pos.x);
    if(abs(currentValue - fragGreen) < handleWidth) {
        if(lightHandle) {
            fragColor = vec4(1.f, 1.f, 1.f, 1.f);
        } else {
            fragColor = vec4(0.f, 0.f, 0.f, 1.f);
        }
        return;
    }

    fragColor = vec4(RGBColor.x, fragGreen, RGBColor.z, 1.f);
}
