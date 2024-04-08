#version 330 core
layout(location = 0) out vec4 fragColor;
in vec3 pos;

uniform vec2 borderSize;
uniform vec4 borderColor;

void main(void) {
    float posXFrac = 0.5f*(1.f + pos.x);
    float posYFrac = 0.5f*(1.f + pos.y);

    bool noBorderH = posXFrac > borderSize.x && posXFrac < 1.f - borderSize.x;
    bool noBorderV = posYFrac > borderSize.y && posYFrac < 1.f - borderSize.y;

    if(noBorderH && noBorderV) {
        fragColor = vec4(0.f, 0.f, 0.f, 0.f);
        return;
    }
    fragColor = borderColor;
}
