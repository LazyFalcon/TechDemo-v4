@export: GGX
float chiGGX(in float v){
    return (v > 0) ? 1.0 : 0.0;
}

float GGX_NormalDistribution(in float NdotH, float alpha){
    float alpha2 = alpha * alpha;
    float NoH2 = NdotH * NdotH;
    float den = NoH2 * alpha2 + (1 - NoH2);
    return (chiGGX(NdotH) * alpha2) / ( PI * den * den );
}

float Geometric_Smith_Schlick_GGX(in float NdV, in float NdL, in float a){
    // Smith schlick-GGX.
    float k = a * 0.5f;
    float GV = NdV / (NdV * (1 - k) + k);
    float GL = NdL / (NdL * (1 - k) + k);

    return GV * GL;
}

float G_GGX_Smith( const in float alpha, const in float dotNL, const in float dotNV ) {
    // geometry term = G(l)⋅G(v) / 4(n⋅l)(n⋅v)
    float a2 = alpha * alpha;
    float gl = dotNL + pow( a2 + ( 1.0 - a2 ) * dotNL * dotNL, 0.5 );
    float gv = dotNV + pow( a2 + ( 1.0 - a2 ) * dotNV * dotNV, 0.5 );

    return 1.0 / ( gl * gv );
} // validated

vec3 Fresnel_Schlick(in float cosT, in vec3 F0){
    return mix(F0, vec3(1.0), pow(1.0 - cosT, 5.0));
}
/// http://blog.selfshadow.com/publications/s2013-shading-course/hoffman/s2013_pbs_physics_math_slides.pdf many functions if you want
/// http://www.alexandre-pestana.com/tweaking-the-cook-torrance-brdf/
/// http://renderwonk.com/publications/s2010-shading-course/hoffman/s2010_physically_based_shading_hoffman_a_notes.pdf
/// + ladna tabelka kolorow przy F0, dla teksturek powinny być brane wartości sRGB, dla metali, reszta ma 0.04
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

    return F*G/denominator*(D*light*lightPowerScale);
}

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

    float s = LdotV - NdotL * NdotV;
    float t = mix(1.0, max(NdotL, NdotV), step(0.0, s));

    float sigma2 = roughness * roughness;
    float A = 1.0 + sigma2 * (albedo / (sigma2 + 0.13) + 0.5 / (sigma2 + 0.33));
    float B = 0.45 * sigma2 / (sigma2 + 0.09);

    return vec3(albedo * max(0.0, NdotL) * (A + B * s / t) / PI);
}
