@vertex:

layout(location=0)in vec4 mVertex;

uniform mat4 uPV;

void main(){
    gl_Position = uProjectionView*mVertex;
}

@fragment:

uniform vec4 uColor;

out vec4 outColor;

void main(void){
    outColor = vuColor;
}
