@vertex:

@import: Uniforms
layout(location=0)in vec4 mVertex;

void main(){
    gl_Position = uPV*mVertex;
}

@fragment:

uniform vec4 uColor;

out vec4 outColor;

void main(void){
    outColor = uColor;
}
