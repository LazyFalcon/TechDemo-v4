#ifdef VERTEX_SHADER

layout(location=0)in vec3 mVertex;
layout(location=1)in vec3 mNormal;
layout(location=2)in vec3 mUV;
layout(location=3)in uint mBoneIndex;
// layout(location=3)in vec4 mTangent;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

layout(std140) uniform uBones {
    mat4 bones[256];
};

out vec3 vUV;
out vec3 vNormalWS;
// out vec3 vTangentWS;
// out vec3 vBinormalWS;

void main(){
    vUV = mUV;
    vUV.z += 0;
    vUV.xy = vec2(0.9);
    // int boneIndex = int(mVertex.w);
    mat4 bone = bones[mBoneIndex];

    vNormalWS = (uModel*bone*vec4(mNormal, 0)).xyz;
    // vTangentWS = (uModel*bone*mTangent).xyz;
    // vBinormalWS = cross(vNormalWS, vTangentWS);

    gl_Position = uProjection*(uView*uModel)*(bone*vec4(mVertex,1));
}

#endif

#ifdef FRAGMENT_SHADER
#extension GL_EXT_texture_array : enable
#extension GL_ARB_shading_language_420pack: enable

out layout(location = 0) vec4 outColor;
out layout(location = 1) vec4 outNormal;

in vec3 vUV;
in vec3 vNormalWS;
// in vec3 vTangentWS;
// in vec3 vBinormalWS;

// layout(binding=0)uniform sampler2DArray uAlbedo;
// layout(binding=1)uniform sampler2DArray uNormalMap;
// layout(binding=2)uniform sampler2DArray uRoughnessMap;
// layout(binding=3)uniform sampler2DArray uMetallicMap;

void main(void){
    outColor.rgb = vec3(0.8);
    // outColor.rgb = texture2DArray(uAlbedo, vUV).rgb;
    // vec3 normalTS = texture2DArray(uNormalMap, vUV*5).rgb*2.0 - 1.0;
    // outNormal.w = texture2DArray(uRoughnessMap, vUV*5).r*2;
    // outColor.a = texture2DArray(uMetallicMap, vUV*5).r*0.5;

    vec3 normalWS = normalize(vNormalWS);
    // vec3 tangentWS = normalize(vTangentWS);
    // vec3 binormalWS = normalize(vBinormalWS);
    // mat3 tangentToWS = mat3(tangentWS, binormalWS, normalWS);
    // normalWS = normalize(tangentToWS*normalTS);

    outNormal.xyz = normalWS;
}
#endif
