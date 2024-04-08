#version 330 core
layout(location = 0) out vec4 fragColor;
in vec3 pos;

uniform vec4 RGBAColor1;
uniform vec4 RGBAColor2;

uniform vec2 meshSize;

void main(void) {
    float posFrac = 0.5f*(1.f + pos.x);
    vec4 colorMix = mix(RGBAColor1, RGBAColor2, posFrac);
    if(colorMix.a < 0.99999f) {
        vec3 meshColor;
        int hId = int(floor(posFrac/meshSize.x));
        int vId = int(floor(0.5f*(1.f + pos.y)/meshSize.y));
        if((hId + vId) % 2 == 0) {
            meshColor = vec3(0.2f, 0.2f, 0.2f);
        } else {
            meshColor = vec3(0.4f, 0.4f, 0.4f);
        }
        fragColor = vec4(mix(meshColor, colorMix.rgb, colorMix.a), 1.f);
        return;
    }
    fragColor = vec4(colorMix.rgb, 1.f);
}
