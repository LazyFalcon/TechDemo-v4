#ifdef VERTEX_SHADER
layout(location=0)in vec4 mVertex;
layout(location=1)in vec4 mPanelBox;
layout(location=2)in float mTextureLayer;
layout(location=3)in float mDepth;
layout(location=4)in vec4 mColor;

uniform float uWidth;
uniform float uHeight;


void main(){
    vec2 position = vec2(mVertex.x*mPanelBox.z, mVertex.y*mPanelBox.w);
    gl_Position = vec4((position+floor(mPanelBox.xy))/vec2(uWidth/2, uHeight/2)-vec2(1), 1-mDepth, 1);
}

#endif

#ifdef FRAGMENT_SHADER

void main(void){
}

#endif
