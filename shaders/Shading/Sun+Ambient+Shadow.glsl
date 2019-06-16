@vertex:

    layout(location=0)in vec4 mVertex;

    void main(){
        gl_Position = (vec4(mVertex.xy, 0, 1));
    }



@fragment:
// #extension GL_EXT_texture_array : enable
// #extension GL_ARB_shading_language_420pack: enable
#extension GL_NV_shadow_samplers_cube : enable
const float PI = 3.1415926;

struct LightSource
{
    vec3 direction;
    vec3 color;
    float energy;
    float lightScale;
};

out layout(location = 0) vec4 outLight;
out layout(location = 1) vec4 outSpecular;

uniform sampler2D uNormal;
uniform sampler2D uDepth;
uniform sampler2D uAlbedo;
uniform samplerCube uCubemap;
// uniform sampler2DArrayShadow uCSMCascades;

uniform LightSource light;

uniform mat4 uInvPV;
uniform vec3 uEye;
uniform vec2 uPixelSize;
// uniform mat4 uCSMProjection[4];
// uniform float uSplitDistances[4];

vec3 getPosition(vec2 uv){
    float depth = 2*texture(uDepth, uv).r-1;
    vec4 viewSpace = vec4(uv*2-1, depth, 1);
    vec4 worldPos = uInvPV*viewSpace;
    worldPos.xyzw /= worldPos.w;
    return worldPos.xyz;
}
vec4 getNormal(vec2 uv){
    return texture(uNormal, uv);
}
vec4 getAlbedo(vec2 uv){
    return texture(uAlbedo, uv);
}
float lightPowerScale = 1;

vec3 sampleEnviroMap(vec3 N, vec3 V, float roughness){
    vec3 reflection = reflect(-V, N);
    return textureLod(uCubemap, reflection*vec3(-1,-1,1), roughness*15).rgb;
}
vec3 samplePrefilteredEnviroMap(vec3 N, float roughness){
    return textureLod(uCubemap, N, roughness).rgb;
}

@import: GGX
/*
    // const float bias = -0.00006;
    // float calcShadow(in vec3 P, in vec2 uv, in float depth, in vec3 N, in int cascade){
    //     float bias = 0.1 + depth/1000;
    //     vec3 positionCorrected = P + N*bias;

    //     vec4 positionLS = (uCSMProjection[cascade]*vec4(positionCorrected, 1))*0.5 + 0.5;
    //     vec2 UVCoords = positionLS.xy;

    //     // float bias = -0.0001*tan(acos(nDotL))*(4-cascade); // cosTheta is dot( n,l ), clamped between 0 and 1
    //     // bias = -clamp(-bias, 0.0, 0.01);

    //     if(UVCoords.x < 0 || UVCoords.x > 1 || UVCoords.y < 0 || UVCoords.y > 1 || positionLS.z > 1) return 1.0;

    //     float xOffset = 1.0/2048.0;
    //     float yOffset = 1.0/2048.0;

    //     float factor = 0.0;

    //     // for (int y = -1 ; y <= 1 ; y++) {
    //     //         for (int x = -1 ; x <= 1 ; x++) {
    //     //                 vec2 Offsets = vec2(x * xOffset, y * yOffset);
    //     //                 vec4 UVC = vec4(UVCoords + Offsets, cascade, positionLS.z +  bias*(1-nDotL));
    //     //                 factor += texture(uCSMCascades, UVC);
    //     //         }
    //     // }
    //     // factor = 0.4 + (factor / 18.0);

    //     // vec4 UVC = vec4(UVCoords, cascade, positionLS.z + bias - (1-nDotL)*0.004);
    //     vec4 UVC = vec4(UVCoords, cascade, positionLS.z);
    //     factor += texture(uCSMCascades, UVC);
    //     factor = 0.4 + (factor / 1.0);

    //     return factor > 0.98 ? 1.0 : factor;
    // }

    // float CombineCSM(in vec3 P, in vec2 uv, in float nDotL, in vec3 N){
    //     if(nDotL < 0.0) return 0;
    //     float depth = distance(P, uEye.xyz);
    //     for(int i=0; i<4; i++){
    //         if(depth < uSplitDistances[i]){
    //             return calcShadow(P, uv, depth, N, i);
    //         }
    //     }
    //     return 1.0;
    // }

    // vec3 colorFromCsm(in vec3 P){
    //     float depth = distance(P, uEye.xyz);
    //     for(int i=0; i<4; i++){
    //         if(depth < uSplitDistances[i]){
    //             if(i==0) return vec3(1,0,0);
    //             if(i==1) return vec3(1,1,0);
    //             if(i==2) return vec3(0,0,1);
    //             if(i==3) return vec3(1,1,1);
    //         }
    //     }
    //     return vec3(0);
    // }
*/
void main(void){
    vec2 uv = gl_FragCoord.xy * uPixelSize;

    vec3 P = getPosition(uv);
    vec4 normal = getNormal(uv);
    vec4 albedo = getAlbedo(uv);
    float metallic = albedo.w;
    vec3 N = normalize(normal.xyz);
    /// TODO: pack in single hfolat
    float roughness = max(normal.w, 0.01);
    roughness *= roughness;

    vec3 V = normalize(uEye - P);
    vec3 L = -normalize(light.direction);
    L = normalize(vec3(0.3, 0.3, 1));
    vec3 kS;

    vec3 specular = calculateSpecular(V, N, L, roughness, metallic, albedo.rgb, kS)*lightPowerScale + sampleEnviroMap(N.xzy, V.xzy, roughness)*exp(-0.1*lightPowerScale)*Fresnel_Schlick(clamp(dot(N,V), 0.0, 1.0), albedo.rgb);
    vec3 irradiance = samplePrefilteredEnviroMap(-N.yzx, 7);
    vec3 diffusePart = 1*light.color*OrenNayar(L, V, N, roughness, 1)*lightPowerScale + irradiance*exp(-0.1*lightPowerScale); /// *ks?

    float shadow = 1;// pow(CombineCSM(P, uv, dot(N, L), N), 2);

    outLight = vec4(diffusePart*(1-kS)*shadow*0.1, 1);
    // outLight = normal*0.5+0.5;
    outSpecular = vec4(specular*shadow*kS*0, 1);
    // outSpecular = vec4(reflect(-V, N) *0.5 + 0.5, 1);
    // outSpecular = vec4(uv.x+uv.y>1? sampleEnviroMap(N.xzy, V.xzy, 0) : N.xzy*0.5+0.5, 1);
    // outSpecular = vec4(N.xzy, 1);
}
