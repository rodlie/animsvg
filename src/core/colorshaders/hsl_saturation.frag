#version 330 core
layout(location = 0) out vec4 fragColor;
in vec3 pos;

uniform vec3 HSLColor;

uniform float currentValue;
uniform float handleWidth; // fraction of width
uniform bool lightHandle;

float hue2rgb(float f1, float f2, float hue) {
    if(hue < 0.0)
        hue += 1.0;
    else if(hue > 1.0)
        hue -= 1.0;
    float res;
    if((6.0 * hue) < 1.0)
        res = f1 + (f2 - f1) * 6.0 * hue;
    else if((2.0 * hue) < 1.0)
        res = f2;
    else if((3.0 * hue) < 2.0)
        res = f1 + (f2 - f1) * ((2.0 / 3.0) - hue) * 6.0;
    else
        res = f1;
    return res;
}

vec3 hsl2rgb(vec3 hsl) {
    vec3 rgb;

    if(hsl.y == 0.0) {
        rgb = vec3(hsl.z); // Luminance
    } else {
        float f2;

        if(hsl.z < 0.5)
            f2 = hsl.z * (1.0 + hsl.y);
        else
            f2 = hsl.z + hsl.y - hsl.y * hsl.z;

        float f1 = 2.0 * hsl.z - f2;

        rgb.r = hue2rgb(f1, f2, hsl.x + (1.0/3.0));
        rgb.g = hue2rgb(f1, f2, hsl.x);
        rgb.b = hue2rgb(f1, f2, hsl.x - (1.0/3.0));
    }
    return rgb;
}

void main(void) {
    float fragHSLSat = 0.5f*(1.f + pos.x);
    if(abs(currentValue - fragHSLSat) < handleWidth) {
        if(lightHandle) {
            fragColor = vec4(1.f, 1.f, 1.f, 1.f);
        } else {
            fragColor = vec4(0.f, 0.f, 0.f, 1.f);
        }
        return;
    }
    vec3 hslColor = vec3(HSLColor.x, fragHSLSat, HSLColor.z);
    fragColor = vec4(hsl2rgb(hslColor), 1.f);
}
