@vertex:

layout(location=0)in vec4 mVertex;

uniform vec4 uScreenImage;

out vec2 vUV;

void main(){
    vUV = mVertex.zw;
    gl_Position = vec4(mVertex.xy*uScreenImage.zw/uScreenImage.xy,0,1);
}



@fragment:
out vec4 outColor;

uniform sampler2D uTexture;

in vec2 vUV;

void main(void){
        outColor = texture(uTexture, vUV);
}
