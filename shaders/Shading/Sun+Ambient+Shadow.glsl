#ifdef VERTEX_SHADER

    layout(location=0)in vec4 mVertex;

    void main(){
        gl_Position = (vec4(mVertex.xy, 0, 1));
    }

#endif

#ifdef FRAGMENT_SHADER
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
    vec3 sampleEnviroMap(vec3 N, vec3 V, float roughness){
        vec3 reflection = reflect(-V, N);
        return texture(uCubemap, reflection*vec3(-1,-1,1), roughness*7).rgb;
    }
    vec3 samplePrefilteredEnviroMap(vec3 N, float roughness){
        return texture(uCubemap, N, roughness).rgb;
    }
    float lightPowerScale = 1;

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

        return F*G/denominator*(D*light*lightPowerScale) + 3* sampleEnviroMap(N.xzy, V.xzy, roughness)*exp(-0.1*lightPowerScale)*Fresnel_Schlick(NdotV, F0);
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

    void main(void){
        vec2 uv = gl_FragCoord.xy * uPixelSize;

        vec3 P = getPosition(uv);
        vec4 normal = getNormal(uv);
        vec4 albedo = getAlbedo(uv);
        float metallic = albedo.w;
        vec3 N = normalize(normal.xyz);
        /// TODO: pack in single hfolat
        float roughness = max(normal.w, 0.01) + 0.2;
        roughness *= roughness;

        vec3 V = normalize(uEye - P);
        vec3 L = -normalize(light.direction);
        L = normalize(vec3(0.3, 0.3, 1));
        vec3 kS;

        vec3 specular = calculateSpecular(V, N, L, roughness, metallic, albedo.rgb, kS)*lightPowerScale;
        vec3 irradiance = samplePrefilteredEnviroMap(-N.yzx, roughness*7)*0.01;
        vec3 diffusePart = 1*light.color*OrenNayar(L, V, N, roughness, 1)*lightPowerScale + irradiance*exp(-0.1*lightPowerScale); /// *ks?

        float shadow = 1;// pow(CombineCSM(P, uv, dot(N, L), N), 2);

        outLight = vec4(diffusePart*(1-kS)*shadow, 1);
        outSpecular = vec4(specular*shadow*kS, 1);
    }

#endif
