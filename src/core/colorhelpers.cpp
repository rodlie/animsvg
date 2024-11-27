/*
#
# Friction - https://friction.graphics
#
# Copyright (c) Ole-André Rodlie and contributors
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# See 'README.md' for more information.
#
*/

// Fork of enve - Copyright (C) 2016-2020 Maurycy Liebner

#include "colorhelpers.h"
#include <math.h>
#include <QString>
#include "glhelpers.h"


bool shouldValPointerBeLightHSV(const float h, const float s, const float v) {
    if(v < 0.6f) return true;
    return h > 0.55f && (s > 0.5f || v < 0.7f);
}

bool shouldValPointerBeLightHSL(float h, float s, float l) {
    hsl_to_hsv(h, s, l);
    return shouldValPointerBeLightHSV(h, s, l);
}

bool shouldValPointerBeLightRGB(float r, float g, float b) {
    rgb_to_hsv_float(r, g, b);
    return shouldValPointerBeLightHSV(r, g, b);
}

//bool isnan(float var)
//{
//    volatile float d = var;
//    return d != d;
//}

void rgb_to_hsv_float(float &r_, float &g_, float &b_) {
    float max, min, delta;
    float h, s, v;
    float r, g, b;

    h = 0.f; // silence gcc warning

    r = r_;
    g = g_;
    b = b_;

    r = CLAMP(r, 0.f, 1.f);
    g = CLAMP(g, 0.f, 1.f);
    b = CLAMP(b, 0.f, 1.f);

    max = MAX3(r, g, b);
    min = MIN3(r, g, b);

    v = max;
    delta = max - min;

    if(delta > 0.0001f) {
        s = delta / max;

        if(r == max) {
            h = (g - b) / delta;
            if(h < 0.f) h += 6.f;
        } else if(g == max) {
            h = 2.f + (b - r) / delta;
        } else if(b == max) {
            h = 4.f + (r - g) / delta;
        }

        h /= 6.f;
    } else {
        s = 0.f;
        h = 0.f;
    }

    r_ = h;
    g_ = s;
    b_ = v;
}

// (from gimp_hsv_to_rgb)
void hsv_to_rgb_float(float &h_, float &s_, float &v_) {
    int i;
    float f, w, q, t;
    float h, s, v;
    float r, g, b;
    r = g = b = 0.0; // silence gcc warning

    h = h_;
    s = s_;
    v = v_;

    h = h - floor(h);
    s = CLAMP(s, 0.f, 1.f);
    v = CLAMP(v, 0.f, 1.f);

    float hue;

    if(s == 0.f) {
        r = v;
        g = v;
        b = v;
    } else {
        hue = h;

        if(hue == 1.f) hue = 0.f;

        hue *= 6.f;

        i = static_cast<int>(floor(hue));
        f = hue - i;
        w = v * (1.f - s);
        q = v * (1.f - (s * f));
        t = v * (1.f - (s * (1.f - f)));

        switch (i) {
        case 0:
            r = v;
            g = t;
            b = w;
            break;
        case 1:
            r = q;
            g = v;
            b = w;
            break;
        case 2:
            r = w;
            g = v;
            b = t;
            break;
        case 3:
            r = w;
            g = q;
            b = v;
            break;
        case 4:
            r = t;
            g = w;
            b = v;
            break;
        case 5:
            r = v;
            g = w;
            b = q;
            break;
        }
    }

    h_ = r;
    s_ = g;
    v_ = b;
}

// (from gimp_rgb_to_hsl)
void rgb_to_hsl_float(float &r_, float &g_, float &b_) {
    float max, min, delta;

    float h, s, l;
    float r, g, b;

    // silence gcc warnings
    h = 0.f;

    r = r_;
    g = g_;
    b = b_;

    r = CLAMP(r, 0.f, 1.f);
    g = CLAMP(g, 0.f, 1.f);
    b = CLAMP(b, 0.f, 1.f);

    max = MAX3(r, g, b);
    min = MIN3(r, g, b);

    l = (max + min) / 2.f;

    if(max == min) {
        s = 0.f;
        h = 0.f; //GIMP_HSL_UNDEFINED;
    } else {
        if(l <= 0.5f)
            s = (max - min) / (max + min);
        else
            s = (max - min) / (2.f - max - min);

        delta = max - min;

        if(delta == 0.f)
        delta = 1.f;

        if(r == max) {
            h = (g - b) / delta;
        } else if(g == max) {
            h = 2.f + (b - r) / delta;
        } else if(b == max) {
            h = 4.f + (r - g) / delta;
        }

        h /= 6.f;

        if(h < 0.f) h += 1.f;
    }

    r_ = h;
    g_ = s;
    b_ = l;
}

static double hsl_value(double n1, double n2, double hue) {
    double val;

    if(hue > 6.0)
        hue -= 6.0;
    else if(hue < 0.0)
        hue += 6.0;

    if(hue < 1.0)
        val = n1 + (n2 - n1) * hue;
    else if(hue < 3.0)
        val = n2;
    else if(hue < 4.0)
        val = n1 + (n2 - n1) * (4.0 - hue);
    else
        val = n1;

    return val;
}


static float hsl_value(float n1, float n2, float hue) {
    float val;

    if(hue > 6.f)
        hue -= 6.f;
    else if(hue < 0.f)
        hue += 6.f;

    if(hue < 1.f)
        val = n1 + (n2 - n1) * hue;
    else if(hue < 3.f)
        val = n2;
    else if(hue < 4.f)
        val = n1 + (n2 - n1) * (4.f - hue);
    else
        val = n1;

    return val;
}

/**
 * gimp_hsl_to_rgb:
 * @hsl: A color value in the HSL colorspace
 * @rgb: The value converted to a value in the RGB colorspace
 *
 * Convert a HSL color value to an RGB color value.
 **/
void hsl_to_rgb_float(float &h_, float &s_, float &l_) {
    float h, s, l;
    float r, g, b;

    h = h_;
    s = s_;
    l = l_;

    h = h - floor(h);
    s = CLAMP(s, 0.f, 1.f);
    l = CLAMP(l, 0.f, 1.f);

    if(s == 0.f) {
        /*  achromatic case  */
        r = l;
        g = l;
        b = l;
    } else {
        float m1, m2;

        if(l <= 0.5f)
            m2 = l * (1.f + s);
        else
            m2 = l + s - l * s;

        m1 = 2.f * l - m2;

        r = hsl_value (m1, m2, h * 6.f + 2.f);
        g = hsl_value (m1, m2, h * 6.f);
        b = hsl_value (m1, m2, h * 6.f - 2.f);
    }

    h_ = r;
    s_ = g;
    l_ = b;
}

void hsv_to_hsl(float &h, float &s, float &v) {
    Q_UNUSED(h)
    float ss = s;
    float vv = v;
    v = (2.f - ss) * vv;
    s = ss * vv;
    s /= (v <= 1.f) ? (v) : 2.f - (v);
    v *= 0.5f;
    if(isnan(s) ) {
        s = 0.f;
    }
}

void hsl_to_hsv(float &h, float &s, float &l) {
    Q_UNUSED(h)
    float ss = s;
    float ll = l;
    ll *= 2.f;
    ss *= (ll <= 1.f) ? ll : 2.f - ll;
    l = (ll + ss) *0.5f;
    s = (2.f * ss) / (ll + ss);
    if(isnan(s)) {
        s = 0.f;
    }
}

void qhsv_to_hsl(qreal &h, qreal &s, qreal &v) {
    Q_UNUSED(h)
    qreal ss = s;
    qreal vv = v;
    v = (2 - ss) * vv;
    s = ss * vv;
    s /= (v <= 1) ? (v) : 2 - (v);
    v *= 0.5;
    if(isnan(s) )
    {
        s = 0.;
    }
}

void qhsl_to_hsv(qreal &h, qreal &s, qreal &l) {
    Q_UNUSED(h)
    qreal ss = s;
    qreal ll = l;
    ll *= 2;
    ss *= (ll <= 1) ? ll : 2 - ll;
    l = (ll + ss) *0.5;
    s = (2 * ss) / (ll + ss);
    if(isnan(s) ) {
        s = 0.;
    }
}

void qhsl_to_rgb(qreal &h_, qreal &s_, qreal &l_) {
    qreal h, s, l;
    qreal r, g, b;

    h = h_;
    s = s_;
    l = l_;

    h = h - floor(h);
    s = CLAMP(s, 0.0, 1.0);
    l = CLAMP(l, 0.0, 1.0);

    if(s == 0.) {
        /*  achromatic case  */
        r = l;
        g = l;
        b = l;
    } else {
        qreal m1, m2;

        if(l <= 0.5)
            m2 = l * (1.0 + s);
        else
            m2 = l + s - l * s;

        m1 = 2.0 * l - m2;

        r = hsl_value (m1, m2, h * 6.0 + 2.0);
        g = hsl_value (m1, m2, h * 6.0);
        b = hsl_value (m1, m2, h * 6.0 - 2.0);
    }

    h_ = r;
    s_ = g;
    l_ = b;
}

void qrgb_to_hsv(qreal &r_, qreal &g_, qreal &b_) {
    qreal max, min, delta;
    qreal h, s, v;
    qreal r, g, b;

    h = 0.0; // silence gcc warning

    r = r_;
    g = g_;
    b = b_;

    r = CLAMP(r, 0.0, 1.0);
    g = CLAMP(g, 0.0, 1.0);
    b = CLAMP(b, 0.0, 1.0);

    max = MAX3(r, g, b);
    min = MIN3(r, g, b);

    v = max;
    delta = max - min;

    if(delta > 0.0001) {
        s = delta / max;

        if(r == max) {
            h = (g - b) / delta;
            if(h < 0.0)
                h += 6.0;
        } else if(g == max) {
            h = 2.0 + (b - r) / delta;
        }
        else if(b == max) {
            h = 4.0 + (r - g) / delta;
        }

        h /= 6.0;
    } else {
        s = 0.0;
        h = 0.0;
    }

    r_ = h;
    g_ = s;
    b_ = v;
}

// (from gimp_hsv_to_rgb)
void qhsv_to_rgb(qreal &h_, qreal &s_, qreal &v_) {
    int i;
    qreal f, w, q, t;
    qreal h, s, v;
    qreal r, g, b;
    r = g = b = 0.0; // silence gcc warning

    h = h_;
    s = s_;
    v = v_;

    h = h - floor(h);
    s = CLAMP(s, 0.0, 1.0);
    v = CLAMP(v, 0.0, 1.0);

    qreal hue;

    if(s == 0.0) {
        r = v;
        g = v;
        b = v;
    } else {
        hue = h;

        if(hue == 1.0)
        hue = 0.0;

        hue *= 6.0;

        i = static_cast<int>(floor(hue));
        f = hue - i;
        w = v * (1.0 - s);
        q = v * (1.0 - (s * f));
        t = v * (1.0 - (s * (1.0 - f)));

        switch (i) {
        case 0:
            r = v;
            g = t;
            b = w;
            break;
        case 1:
            r = q;
            g = v;
            b = w;
            break;
        case 2:
            r = w;
            g = v;
            b = t;
            break;
        case 3:
            r = w;
            g = q;
            b = v;
            break;
        case 4:
            r = t;
            g = w;
            b = v;
            break;
        case 5:
            r = v;
            g = w;
            b = q;
            break;
        }
    }

    h_ = r;
    s_ = g;
    v_ = b;
}

// (from gimp_rgb_to_hsl)
void qrgb_to_hsl(qreal &r_, qreal &g_, qreal &b_) {
    qreal max, min, delta;

    qreal h, s, l;
    qreal r, g, b;

    // silence gcc warnings
    h = 0;

    r = r_;
    g = g_;
    b = b_;

    r = CLAMP(r, 0.0, 1.0);
    g = CLAMP(g, 0.0, 1.0);
    b = CLAMP(b, 0.0, 1.0);

    max = MAX3(r, g, b);
    min = MIN3(r, g, b);

    l = (max + min) / 2.0;

    if(max == min) {
        s = 0.0;
        h = 0.0; //GIMP_HSL_UNDEFINED;
    } else {
        if(l <= 0.5)
            s = (max - min) / (max + min);
        else
            s = (max - min) / (2.0 - max - min);

        delta = max - min;

        if(delta == 0.0) delta = 1.0;

        if(r == max) {
            h = (g - b) / delta;
        } else if(g == max) {
            h = 2.0 + (b - r) / delta;
        } else if(b == max) {
            h = 4.0 + (r - g) / delta;
        }

        h /= 6.0;

        if(h < 0.0) h += 1.0;
    }

    r_ = h;
    g_ = s;
    b_ = l;
}
