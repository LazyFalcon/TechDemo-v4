#ifdef VERTEX_SHADER

layout(location=0)in vec4 mVertex;

uniform mat4 uPV;
uniform mat4 uTransform;
uniform float uLightScale;

void main(){
    vec4 V = mVertex;
    gl_Position = uPV*uTransform*V;
}

#endif

#ifdef FRAGMENT_SHADER
const float PI = 3.1415926;
float lightPowerScale = 1;

struct LightSource
{
    vec3 position;
    vec3 color;
    float fallof;
    float energy;
    vec3 direction;
    float cosAngle;
};


out layout(location = 0) vec4 outLight;
out layout(location = 1) vec4 outSpecular;

uniform sampler2D uNormal;
uniform sampler2D uDepth;

uniform LightSource light;

uniform mat4 uInvPV;
uniform vec3 uEye;
uniform vec2 uPixelSize;

vec3 getPosition(in vec2 uv){
    float depth = 2*texture(uDepth, uv).r-1;
    vec4 viewSpace = vec4(uv*2-1, depth, 1);
    vec4 worldPos = uInvPV*viewSpace;
    worldPos.xyzw /= worldPos.w;
    return worldPos.xyz;
}
vec4 getNormal(in vec2 uv){
    return texture(uNormal, uv);
}

float invSquareIntensity(in float E, in float x, in float r){
    float att = max(0, 1 - (x*x)/(r*r));
    return att*att * E*5;

    // float denom = D/r + 1;
    // float attenuation = 1 / (denom*denom);
    // attenuation = (attenuation - light.fallof) / (1 - light.fallof);
    // attenuation = max(attenuation, 0);
    // return max(attenuation, 0);
    // return E * (D*D / (D*D + r*r));
}

/// specular
float chiGGX(float v){
    return (v > 0) ? 1.0 : 0.0;
}
float GGX_NormalDistribution(in float NdotH, float alpha){
    float alpha2 = alpha * alpha;
    float NoH2 = NdotH * NdotH;
    float den = NoH2 * alpha2 + (1 - NoH2);
    return (chiGGX(NdotH) * alpha2) / ( PI * den * den );
}
float Geometric_Smith_Schlick_GGX(float NdV, float NdL, float a){
    // Smith schlick-GGX.
    float k = a * 0.5f;
    float GV = NdV / (NdV * (1 - k) + k);
    float GL = NdL / (NdL * (1 - k) + k);

    return GV * GL;
}
vec3 Fresnel_Schlick(in float cosT, in vec3 F0){
    return mix(F0, vec3(1.0), pow(1.0 - cosT, 5.0));
}
vec3 calculateSpecular(in vec3 V, in vec3 N, in vec3 L, in float roughness, in float metallic, in vec3 F0, inout vec3 kS){
    vec3 H = normalize(L + V);
    float NdotL = clamp(dot(N,L), 0.0, 1.0);
    float NdotV = clamp(dot(N,V), 0.0, 1.0);
    float NdotH = clamp(dot(N,H), 0.0, 1.0);
    float LdotH = clamp(dot(L,H), 0.0, 1.0);
    float VdotH = clamp(dot(V,H), 0.0, 1.0);
    float VdotL = clamp(dot(V,L), 0.0, 1.0);

    // mix between metal and non-metal material, for non-metal
    // constant base specular factor of 0.04 grey is used
    F0 = mix(vec3(0.04), F0, metallic);

    vec3 F = Fresnel_Schlick(VdotH, F0);
    float D = GGX_NormalDistribution(NdotH, roughness);
    float G = Geometric_Smith_Schlick_GGX(NdotV, NdotL, roughness);
    G = G*G;
    float denominator = clamp( 4 * (NdotV * NdotH + 0.05), 0, 1);
    kS = F;
    float light = max(dot(L, N), 0.01);
    light = 1-exp(-pow((5*light),4));

    // return vec3(1-F);
    // return vec3(D);
    return F*G/denominator*(D*light*lightPowerScale);
}
/// diffuse, different verions
vec3 OrenNayar(vec3 L, vec3 V, vec3 N, float roughness, float albedo){
    /// http://mimosa-pudica.net/improved-oren-nayar.html
    /// http://shaderjvo.blogspot.com/2011/08/van-ouwerkerks-rewrite-of-oren-nayar.html
    float LdotV = max(dot(L, V), 0.01);
    float NdotL = max(dot(L, N), 0.01);
    float NdotV = max(dot(N, V), 0.01);
    float r2 = roughness * roughness;

    vec2 oren_nayar_fraction = r2 / (r2 + vec2(0.33, 0.09));
    vec2 oren_nayar = vec2(1, 0) + vec2(-0.5, 0.45) * oren_nayar_fraction;

    vec2 cos_theta = clamp(vec2(NdotL, NdotV), 0, 1);
    vec2 cos_theta2 = cos_theta * cos_theta;

    // float sin_theta = sqrt((1-cos_theta2.x) * (1-cos_theta2.y));
    // vec3 light_plane = normalize(L - cos_theta.x * N);
    // vec3 view_plane = normalize(V - cos_theta.y * N);
    // float cos_phi = clamp(dot(light_plane, view_plane), 0, 1);
    // // Composition
    // float diffuse_oren_nayar = cos_phi * sin_theta / max(cos_theta.x, cos_theta.y);
    // float diffuse = cos_theta.x * (oren_nayar.x + oren_nayar.y * diffuse_oren_nayar);
    //
    // return vec3(clamp(diffuse, 0, 100)*albedo);

    float s = LdotV - NdotL * NdotV;
    float t = mix(1.0, max(NdotL, NdotV), step(0.0, s));

    float sigma2 = roughness * roughness;
    float A = 1.0 + sigma2 * (albedo / (sigma2 + 0.13) + 0.5 / (sigma2 + 0.33));
    float B = 0.45 * sigma2 / (sigma2 + 0.09);

    return vec3(albedo * max(0.0, NdotL) * (A + B * s / t) / PI);
}

/// https://www.blender.org/manual/render/blender_render/lighting/lamps/spot/introduction.html
void main(void){
    vec2 uv = gl_FragCoord.xy * uPixelSize;

    vec3 P = getPosition(uv);
    vec4 normal = getNormal(uv);
    vec3 N = normalize(normal.xyz);
    float roughness = normal.w;
    float metallic = 0;
    vec4 albedo = vec4(1);

    vec3 V = normalize(uEye - P);
    vec3 L = light.position - P;
    float x = length(L);
    L /= x;
    float blend = 0.9986f;
    float cosAngle = max(dot(L, -light.direction), 0);
    // float xf = light.cosAngle > cosAngle ? 0 : 1;
    float xf = smoothstep(light.cosAngle, light.cosAngle + (1-light.cosAngle)*blend, cosAngle);

    // float I =  xf;
    float I = invSquareIntensity(light.energy, x, light.fallof) * xf;
    vec3 kS;

    vec3 specular = calculateSpecular(V, N, L, roughness, metallic, albedo.rgb, kS) * I;
    vec3 diffusePart = light.color*OrenNayar(L, V, N, roughness, 1)*lightPowerScale * I;

    // outLight = vec4(vec3(xf), 1);
    outLight = vec4(diffusePart*(1-kS), 1);
    outSpecular = vec4(specular*light.color, 1);
}


#endif
