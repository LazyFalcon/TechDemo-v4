@vertex:
layout(location=0)in vec4 mVertex;

out vec2 vUV;

void main(){
    vUV = mVertex.zw;
    gl_Position = vec4(mVertex.xy,0,1);
}

@fragment:
out vec4 outColor;

uniform vec3 uStrenght;
uniform vec2 uPixelSize;
uniform sampler2D uTexture;

in vec2 vUV;

void main(void){
    vec2 vec = (vUV*(1-8*uPixelSize) - vec2(0.5))*2*uPixelSize;
    outColor.r = texture(uTexture, vUV*(1-8*uPixelSize) + vec2(uPixelSize*4) + vec*uStrenght.x*4).r;
    outColor.g = texture(uTexture, vUV*(1-8*uPixelSize) + vec2(uPixelSize*4) + vec*uStrenght.y*4).g;
    outColor.b = texture(uTexture, vUV*(1-8*uPixelSize) + vec2(uPixelSize*4) + vec*uStrenght.z*4).b;
    outColor.a = 1;
}
