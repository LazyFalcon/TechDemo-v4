@vertex:

layout(location=0)in vec4 mVertex;

uniform mat4 uPV;
uniform float uSize;
uniform vec4 uPosition;

void main(){
    gl_Position = uPV*vec4(uSize*mVertex.xyz*3 + uPosition.xyz, 1);
}

@fragment:
const float PI = 3.1415926;
float lightPowerScale = 1;

struct LightSource
{
    vec3 position;
    vec3 color;
    float fallof;
    float energy;
    float lightScale;
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
    float att = 1 / (1 + 1*x + 100*x*x*0) * max(0, 1-(x)/(r)) * max(0, 1-(x)/(r));
    return att;
}
float sphericalBound(float I, float D, float r){
    return clamp(I * (D - r) / D, 0, 100);
}

@import: GGX

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

    float I = max(invSquareIntensity(light.energy, x, light.fallof*3), 0);
    // float I = 1;
    // I = sphericalBound(I, light.fallof, r);

    vec3 kS;
    float selfShadowing = (dot(N,L) > 0) ? 1 : 0;
    vec3 diffusePart = vec3(1)*OrenNayar(L, V, N, roughness, 1)*lightPowerScale * I * selfShadowing;
    // vec3 diffusePart = light.color*OrenNayar(L, V, N, roughness, 1)*lightPowerScale * I * selfShadowing;
    vec3 specular = calculateSpecular(V, N, L, roughness, metallic, albedo.rgb, kS) * I * selfShadowing;

    outLight = vec4(diffusePart*(1-kS)*100, 1);
    outSpecular = vec4(specular*light.color, 1);
}
