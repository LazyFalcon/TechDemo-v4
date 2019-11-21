@vertex:

layout(location=0)in vec3 mVertex;
layout(location=3)in uint mBoneIndex;

uniform mat4 uModel;

layout(std140) uniform uBones {
    mat4 bones[256];
};

void main(){

    // int boneIndex = int(mVertex.w);
    mat4 bone = bones[mBoneIndex];

    vNormalWS = (uModel*bone*vec4(mNormal, 0)).xyz;
    // vTangentWS = (uModel*bone*mTangent).xyz;
    // vBinormalWS = cross(vNormalWS, vTangentWS);

    gl_Position = bone*vec4(mVertex,1);
}

@geometry:

layout(triangles, invocations = 4) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 uMatrices[4];

void main(){
    gl_Layer = gl_InvocationID;
    for(int i = 0; i < gl_in.length(); ++i){
        gl_Position = uMatrices[gl_InvocationID] * gl_in[i].gl_Position;
        gl_Position.z = clamp(gl_Position.z, -1, 1);
        EmitVertex();
    }

    EndPrimitive();
}

@fragment:
void main(void){}
