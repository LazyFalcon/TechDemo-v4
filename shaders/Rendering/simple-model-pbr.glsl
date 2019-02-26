@vertex:

layout(location=0)in vec3 mVertex;
layout(location=1)in vec3 mNormal;
layout(location=2)in vec3 mUV;
layout(location=3)in vec3 mColor;
layout(location=4)in float mRoughness;
layout(location=5)in float mMetallic;

uniform mat4 uPV;

layout(std140) uniform uBones {
    mat4 bones[256];
};

out vec3 vUV;
out vec3 vColor;
out vec3 vNormalWS;
out float vRoughness;
out float vMetallic;

void main(){
    vUV = mUV*6;
    vUV.z = 5; //gl_DrawID%7;
    vColor = mColor;
    vRoughness = mRoughness;
    vMetallic = mMetallic;
    vNormalWS = (bones[gl_DrawID]*vec4(mNormal, 0)).xyz;

    gl_Position = uPV*(bones[gl_DrawID]*vec4(mVertex, 1));
}



@fragment:
#extension GL_EXT_texture_array : enable
#extension GL_ARB_shading_language_420pack: enable

out layout(location = 0) vec4 outColor;
out layout(location = 1) vec4 outNormal;

layout(binding=0)uniform sampler2DArray uAlbedo;
layout(binding=1)uniform sampler2DArray uRoughnessMap;
layout(binding=2)uniform sampler2DArray uMetallicMap;

in vec3 vUV;
in vec3 vColor;
in vec3 vNormalWS;
in float vRoughness;
in float vMetallic;

void main(void){
    outColor.rgb = pow(mix(vColor, texture2DArray(uAlbedo, vUV).rgb, 0.05), vec3(0.5));
    float mixVal = 0.7;
    outColor.a = mix(vMetallic, texture2DArray(uMetallicMap, vUV).r, mixVal);
    outNormal.w = mix(vRoughness, texture2DArray(uRoughnessMap, vUV).r, mixVal);
    outNormal.xyz = normalize(vNormalWS + vNormalWS.zxy*outNormal.w*0.0);
}
