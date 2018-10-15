@vertex:
layout(location=0)in vec4 mVertex;
layout(location=1)in vec4 mPanelBox;
layout(location=2)in float mTextureLayer;
layout(location=3)in float mDepth;
layout(location=4)in vec4 mColor;

uniform float uWidth;
uniform float uHeight;

out vec4 vColor;
out float vTextureLayer;
out float d;

void main(){
    vColor = mColor.abgr;
    vTextureLayer = mTextureLayer;
    d = mDepth;
    vec2 position = vec2(mVertex.x*mPanelBox.z, mVertex.y*mPanelBox.w);
    gl_Position = vec4((position+floor(mPanelBox.xy))/vec2(uWidth/2, uHeight/2)-vec2(1), 1-mDepth, 1);
}



@fragment:
out vec4 outColor;

in vec4 vColor;
in float vTextureLayer;
in float d;

void main(void){
    outColor = vec4(vColor.rgb*vColor.a, vColor.a);
}
