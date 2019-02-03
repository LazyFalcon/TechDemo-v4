@vertex:

layout(location=0)in vec3 mVertex;

uniform mat4 uModel;
uniform mat4 uProjectionView;

out float vZ;

void main(){
    gl_Position = uProjectionView*(uModel*vec4(mVertex, 1));
    vZ = gl_Position.z;
}



@fragment:
#extension GL_EXT_texture_array : enable
#extension GL_ARB_shading_language_420pack: enable

out layout(location = 0) float outDepth;

in float vZ;

void main(void){
    outDepth = vZ;
}
