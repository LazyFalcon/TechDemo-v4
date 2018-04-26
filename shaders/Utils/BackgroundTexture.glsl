#ifdef VERTEX_SHADER

layout(location=0)in vec4 mVertex;


out vec2 vUV;

void main(){
    vUV = mVertex.zw;
    gl_Position = vec4(mVertex.xy,0,1);
}

#endif

#ifdef FRAGMENT_SHADER
out vec4 outColor;

uniform sampler2D uTexture;

in vec2 vUV;

void main(void){
    outColor = texture(uTexture, vUV);
}

#endif
