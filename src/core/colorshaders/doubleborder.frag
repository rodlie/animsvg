#version 330 core
layout(location = 0) out vec4 fragColor;
in vec3 pos;

uniform vec2 outerBorderSize;
uniform vec2 innerBorderSize;

uniform vec4 outerBorderColor;
uniform vec4 innerBorderColor;

void main(void) {
    float posXFrac = 0.5f*(1.f + pos.x);
    float posYFrac = 0.5f*(1.f + pos.y);

    vec2 combinedBorderSize = outerBorderSize + innerBorderSize;

    bool noBorderH = posXFrac > combinedBorderSize.x &&
            posXFrac < 1.f - combinedBorderSize.x;
    bool noBorderV = posYFrac > combinedBorderSize.y &&
            posYFrac < 1.f - combinedBorderSize.y;

    if(noBorderH && noBorderV) {
        fragColor = vec4(0.f, 0.f, 0.f, 0.f);
        return;
    }

    bool innerBorderH = posXFrac > outerBorderSize.x &&
            posXFrac < 1.f - outerBorderSize.x;
    bool innerBorderV = posYFrac > outerBorderSize.y &&
            posYFrac < 1.f - outerBorderSize.y;

    if(innerBorderH && innerBorderV) {
        fragColor = innerBorderColor;
        return;
    }
    fragColor = outerBorderColor;
}
