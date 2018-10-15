@vertex:
layout(location=0)in vec4 mVertex;

out vec2 vUV;
void main(){
    vUV = mVertex.zw;
    gl_Position = vec4(mVertex.xy, 1, 1);
}



@fragment:


out vec4 outColor;

uniform sampler2D uTexture;

in vec2 vUV;

void main(){
    outColor = vec4(texture(uTexture, vUV).r);
}
