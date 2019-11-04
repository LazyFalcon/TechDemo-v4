#include "core.hpp"
#include "Colors.hpp"
#include <cmath>

/*
vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}
*/

// r,g,b values are from 0 to 1
// h = [0,360], s = [0,1], v = [0,1]
//        if s == 0, then h = -1 (undefined)
namespace color
{
struct Color
{
    union
    {
        u32 color;

    } rgba;
};

struct RGBA
{
    union
    {
        u32 rgba;
        struct
        {
            u8 a;
            u8 b;
            u8 g;
            u8 r;
        };
    };
};

union TmpColor
{
    u32 rgba;
    struct
    {
        u8 a;
        u8 b;
        u8 g;
        u8 r;
    };
};

uint32_t hexFromHSV(float h, float s, float v, float a) {
    TmpColor color;
    color.rgba = 0;
    color.a = a * 0xff;

    if(s == 0.f) {
        // achromatic (grey)
        color.r = color.g = color.b = v * 0xff;
        return color.rgba;
    }
    h /= 60; // sector 0 to 5
    int i = floor(h);
    float f = h - i; // factorial part of h
    float p = v * (1 - s);
    float q = v * (1 - s * f);
    float t = v * (1 - s * (1 - f));

    switch(i) {
        case 0:
            color.r = v * 0xff;
            color.g = t * 0xff;
            color.b = p * 0xff;
            break;
        case 1:
            color.r = q * 0xff;
            color.g = v * 0xff;
            color.b = p * 0xff;
            break;
        case 2:
            color.r = p * 0xff;
            color.g = v * 0xff;
            color.b = t * 0xff;
            break;
        case 3:
            color.r = p * 0xff;
            color.g = q * 0xff;
            color.b = v * 0xff;
            break;
        case 4:
            color.r = t * 0xff;
            color.g = p * 0xff;
            color.b = v * 0xff;
            break;
        default: // case 5:
            color.r = v * 0xff;
            color.g = p * 0xff;
            color.b = q * 0xff;
            break;
    }
    return color.rgba;
}

// void RGBtoHSV( float r, float g, float b, float *h, float *s, float *v )
// {
//     // float min, max, delta;
//     //
//     // min = MIN( r, g, b );
//     // max = MAX( r, g, b );
//     // *v = max;                // v
//     //
//     // delta = max - min;
//     //
//     // if( max != 0 )
//     //     *s = delta / max;        // s
//     // else {
//     //     // r = g = b = 0        // s = 0, v is undefined
//     //     *s = 0;
//     //     *h = -1;
//     //     return;
//     // }
//     //
//     // if( r == max )
//     //     *h = ( g - b ) / delta;        // between yellow & magenta
//     // else if( g == max )
//     //     *h = 2 + ( b - r ) / delta;    // between cyan & yellow
//     // else
//     //     *h = 4 + ( r - g ) / delta;    // between magenta & cyan
//     //
//     // *h *= 60;                // degrees
//     // if( *h < 0 )
//     //     *h += 360;

// }

// void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v )
// {
//     // int i;
//     // float f, p, q, t;
//     //
//     // if( s == 0 ) {
//     //     // achromatic (grey)
//     //     *r = *g = *b = v;
//     //     return;
//     // }
//     //
//     // h /= 60;            // sector 0 to 5
//     // i = floor( h );
//     // f = h - i;            // factorial part of h
//     // p = v * ( 1 - s );
//     // q = v * ( 1 - s * f );
//     // t = v * ( 1 - s * ( 1 - f ) );
//     //
//     // switch( i ) {
//     //     case 0:
//     //         *r = v;
//     //         *g = t;
//     //         *b = p;
//     //         break;
//     //     case 1:
//     //         *r = q;
//     //         *g = v;
//     //         *b = p;
//     //         break;
//     //     case 2:
//     //         *r = p;
//     //         *g = v;
//     //         *b = t;
//     //         break;
//     //     case 3:
//     //         *r = p;
//     //         *g = q;
//     //         *b = v;
//     //         break;
//     //     case 4:
//     //         *r = t;
//     //         *g = p;
//     //         *b = v;
//     //         break;
//     //     default:        // case 5:
//     //         *r = v;
//     //         *g = p;
//     //         *b = q;
//     //         break;
//     // }

// }
}
