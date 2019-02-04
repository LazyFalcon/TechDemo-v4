@vertex:

layout(location=0)in vec3 mVertex;

uniform mat4 uProjectionView;
uniform mat4 uModel;

out float vZ;

void main(){
    gl_Position = uProjectionView*(uModel*vec4(mVertex, 1));
}

@fragment:
// out vec4 outColor;
void main(void){
    // outColor = vec4(1,0,0,1);
}
