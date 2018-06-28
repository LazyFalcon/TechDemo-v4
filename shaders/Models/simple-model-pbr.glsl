#ifdef VERTEX_SHADER

layout(location=0)in vec4 mVertex;
layout(location=1)in vec3 mUV;
layout(location=2)in vec4 mNormal;
// layout(location=3)in vec4 mTangent;

uniform mat4 uPV;

layout(std140) uniform uBones {
    mat4 bones[256];
};

out vec3 vUV;
out vec3 vNormalWS;

void main(){
    vUV = mUV;
    vNormalWS = (bones[gl_DrawID]*mNormal).xyz;

    gl_Position = uPV*(bones[gl_DrawID]*mVertex);
}

#endif

#ifdef FRAGMENT_SHADER
#extension GL_EXT_texture_array : enable
#extension GL_ARB_shading_language_420pack: enable

out layout(location = 0) vec4 outColor;
out layout(location = 1) vec4 outNormal;

layout(binding=0)uniform sampler2DArray uAlbedo;
layout(binding=1)uniform sampler2DArray uRoughnessMap;
layout(binding=2)uniform sampler2DArray uMetallicMap;

in vec3 vUV;
in vec3 vNormalWS;

void main(void){
    outColor.rgb = vec3(1);
    // outColor.rgb = texture2DArray(uAlbedo, vUV).rgb;
    outNormal.w = texture2DArray(uRoughnessMap, vUV*5).r*0.8;
    outColor.a = texture2DArray(uMetallicMap, vUV*5).r;

    outNormal.xyz = normalize(vNormalWS);
}

#endif
