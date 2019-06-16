@vertex:

layout(location=0)in vec3 mVertex;
layout(location=1)in vec3 mNormal;
layout(location=2)in vec3 mUV;
layout(location=3)in uint mBoneIndex;

layout(location=4)in vec3 mColor;
layout(location=5)in float mRoughness;
layout(location=6)in float mMetallic;
layout(location=7)in float mSpecular;
layout(location=8)in float mAnisotropic;
layout(location=9)in float mClearcoat;
layout(location=10)in float mEmissive;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

layout(std140) uniform uBones {
    mat4 bones[256];
};

out vec3 vUV;
out vec3 vNormalWS;
out vec3 vColor;
out float vRoughness;
out float vMetallic;
out float vSpecular;
out float vAnisotropic;
out float vClearcoat;
out float vEmissive;
// out vec3 vTangentWS;
// out vec3 vBinormalWS;

void main(){
    vUV = mUV;
    vColor = mColor;
    vRoughness = mRoughness;
    vMetallic = mMetallic;
    vSpecular = mSpecular;
    vAnisotropic = mAnisotropic;
    vClearcoat = mClearcoat;
    vEmissive = mEmissive;

    // int boneIndex = int(mVertex.w);
    mat4 bone = bones[mBoneIndex];

    vNormalWS = (uModel*bone*vec4(mNormal, 0)).xyz;
    // vTangentWS = (uModel*bone*mTangent).xyz;
    // vBinormalWS = cross(vNormalWS, vTangentWS);

    gl_Position = uProjection*(uView*uModel)*(bone*vec4(mVertex,1));
}



@fragment:
#extension GL_EXT_texture_array : enable
#extension GL_ARB_shading_language_420pack: enable

out layout(location = 0) vec4 outColor;
out layout(location = 1) vec4 outNormal;
// out layout(location = 2) vec4 outEmissive;

in vec3 vUV;
in vec3 vNormalWS;
in vec3 vColor;
in float vRoughness;
in float vMetallic;
in float vSpecular;
in float vAnisotropic;
in float vClearcoat;
// in vec3 vTangentWS;
// in vec3 vBinormalWS;

// layout(binding=0)uniform sampler2DArray uAlbedo;
// layout(binding=1)uniform sampler2DArray uNormalMap;
// layout(binding=2)uniform sampler2DArray uRoughnessMap;
// layout(binding=3)uniform sampler2DArray uMetallicMap;

void main(void){
    outColor.rgb = vColor;
    outNormal.w = vRoughness;
    outColor.a = vMetallic;
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
