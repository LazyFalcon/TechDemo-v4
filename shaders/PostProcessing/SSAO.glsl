#ifdef VERTEX_SHADER

layout(location=0)in vec4 mVertex;


out vec2 vUV;

void main(){
    vUV = mVertex.zw;
    gl_Position = vec4(mVertex.xy, 0, 1);
}

#endif
/// http://www.gamedev.net/topic/550452-ssgi-round-2/
/// depth reconstruction: https://mynameismjp.wordpress.com/2009/05/05/reconstructing-position-from-depth-continued/
#ifdef FRAGMENT_SHADER
/*
    TODO:
*/


out vec2 outAODepth;

uniform sampler2D uDepth;
uniform sampler2D uNormal;
uniform sampler2D uSSAONoise;

uniform mat4 uView;
uniform mat4 uInvPV;
uniform vec2 uScreenSize;
uniform vec2 uPixelSize;
uniform float uFovTan;
uniform float uNear;
uniform float uFar;
uniform vec4 uEyePosition;
in vec2 vUV;

vec3 getPosition(in vec2 uv){
    float depth = 2*texture(uDepth, uv).r-1;
    vec4 viewSpace = vec4(uv*2-1, depth, 1);
    vec4 worldPos = uInvPV*viewSpace;
    worldPos.xyzw /= worldPos.w;
    return worldPos.xyz;
}
vec3 getPositionDepth(in vec2 uv, out float d){
    float depth = 2*texture(uDepth, uv).r-1;
    d = (2. * uNear*uFar) / (uNear + uFar - depth * (uFar - uNear));
    vec4 viewSpace = vec4(uv*2-1, depth, 1);
    vec4 worldPos = uInvPV*viewSpace;
    worldPos.xyzw /= worldPos.w;
    return worldPos.xyz;
}
vec3 getNormal(in vec2 uv){
    return normalize(texture(uNormal, uv).xyz);
}

vec2 getRandom(in vec2 uv){
    return normalize(texture(uSSAONoise, uv)).xy;
}

const float bias = 0.005;
float computeAmbientOcclusion(vec2 uv, vec2 dCoord, vec3 position, vec3 normal, in float radius, in float refDepth){
    float depth;
    vec3 diff = getPositionDepth(uv + dCoord, depth) - position;
    float d = length(diff);
    vec3 v = diff / d;
    float rangeCheck = depth < refDepth ? 1.0 : 0.0;
    float ao = max(0.0, dot(normal,v) - bias) * INTENSITY * (1.0/(1.0+d))*2;
    return ao;
}

vec2 getRadius(float d){
    return clamp(uScreenSize.x * RADIUS/(d*uFovTan), MIN_KERNEL, MAX_KERNEL) * uPixelSize * 2;
    // return clamp(uScreenSize.x * 0.19/(d*uFovTan), 5, 800) * uPixelSize;
    // return min(uScreenSize.x*uPixelSize * 0.12/(d*uFovTan), 0.2);
}

// const vec2 kernel_2[4] = vec2[] (vec2(1,1), vec2(-1,1),    vec2(1,-11), vec2(-1,-1));
// const vec2 kernel_2[4] = vec2[] (vec2(1,0), vec2(-1,0),    vec2(0,1), vec2(0,-1));

const vec2 kernel_2[16] = vec2[](
    vec2(0.1269964f, 0.5985349f),
    vec2(-0.4555801f, 0.1565776f),
    vec2(0.5543445f, 0.07544823f),
    vec2(0.718349f, 0.4695161f),
    vec2(-0.06338997f, -0.1280612f),
    vec2(0.06348574f, -0.5134893f),
    vec2(0.570318f, -0.4444983f),
    vec2(0.9654415f, -0.2006874f),
    vec2(-0.2935757f, 0.7645917f),
    vec2(-0.8531411f, 0.2207384f),
    vec2(0.4822471f, 0.8707393f),
    vec2(-0.8665535f, -0.4418707f),
    vec2(-0.7099477f, 0.6174989f),
    vec2(-0.4009017f, -0.3565602f),
    vec2(0.01245382f, -0.9724864f),
    vec2(-0.4312304f, -0.7819526f)
    );
// const vec2 kernel_2[16] = vec2[](
    // vec2(0.6685306f, 0.3039912f),
    // vec2(0.5430672f, 0.6923575f),
    // vec2(0.006410657f, 0.4063535f),
    // vec2(0.4211033f, -0.3016995f),
    // vec2(0.8689159f, -0.04662216f),
    // vec2(-0.02319174f, -0.1191266f),
    // vec2(0.1479476f, -0.683134f),
    // vec2(0.6145651f, -0.7408658f),
    // vec2(-0.4873389f, 0.1691543f),
    // vec2(-0.257201f, 0.9284028f),
    // vec2(-0.6043539f, 0.5890929f),
    // vec2(-0.6788281f, -0.2122179f),
    // vec2(-0.2844881f, -0.541756f),
    // vec2(-0.9309591f, 0.2976663f),
    // vec2(0.297691f, 0.1394002f),
    // vec2(0.1623268f, 0.859301)
    // );

void main(void){
    float ao = 0.0;
    float depth; /// [0 -> 1]
    vec3 position = getPositionDepth(vUV, depth);
    if(depth > 1500.f) outAODepth = vec2(1);
    else {
        vec3 normal = getNormal(vUV);
        // vec2 radius = getRadius(depth)*7;
        vec2 radius = uPixelSize*400/(depth*uFovTan);
        vec2 rand = getRandom(vUV*3);

        const int iterations = 16;
        for(int i=0; i<iterations; ++i){

            vec2 coord1 = reflect(kernel_2[i], rand)*radius;//+ rand*uPixelSize*3;
            // vec2 coord1 = kernel_2[i]*radius;//+ rand*uPixelSize*3;
            vec2 coord2 = vec2(coord1.x*0.707 - coord1.y*0.707, coord1.x*0.707 + coord1.y*0.707);
            // coord2 = kernel_2[i]*radius;
            // ao += computeAmbientOcclusion(vUV, coord1*0.25, position, normal, radius.x, depth);
            // ao += computeAmbientOcclusion(vUV, coord2*0.5, position, normal, radius.x, depth);
            // ao += computeAmbientOcclusion(vUV, coord1*0.75, position, normal, radius.x, depth);
            ao += computeAmbientOcclusion(vUV, coord2*0.5, position, normal, radius.x, depth)*0.5;
            ao += computeAmbientOcclusion(vUV, coord1, position, normal, radius.x, depth);
            // ao += computeAmbientOcclusion(vUV, kernel_2[i]*radius, position, normal);
        }

        float intensityCorrection = sqrt(depth)/2.1005;
        ao /= float(iterations)*intensityCorrection * 2;
        // ao /= float(iterations)*4;
        // ao = clamp(ao*10, 0,1);
        // intensityCorrection = 1;
        outAODepth = vec2(1-clamp(ao, 0, 1), intensityCorrection);
    }
}


#endif
