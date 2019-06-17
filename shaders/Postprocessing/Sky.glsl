
@vertex:
layout(location=0)in vec4 mVertex;
layout(location=1)in vec2 mUV;
/// https://github.com/TPZF/GlobWeb/tree/master/shaders
uniform mat4 uPV;
uniform mat4 uModel;
uniform mat4 uPlanetMatrix;

uniform float uCameraHeight;
uniform float uSkyRadius;
uniform float uGroundRadius;
uniform float uScaleDepth;

out vec3 vCameraDirection;
out vec3 vCameraPosition;
out float vDistance;

void main(){
    vCameraPosition = vec3(0, 0, uGroundRadius + uCameraHeight/500000 - 0.01);
    vCameraDirection = normalize((uPlanetMatrix*mVertex).xyz+vec3(0,0,0.1));

    float angle = dot(vCameraDirection, vec3(0,0,-1));
    float B =  - 2 * vCameraPosition.z * dot(vCameraDirection, vec3(0,0,-1));
    float C = pow(vCameraPosition.z, 2) - pow(uSkyRadius, 2);
    float delta = B*B - 4*C;
    vDistance = (B - sqrt(delta)) / C * 0.5f;

    mat4 tmp = uPV*uModel;
    vec4 position = tmp*uPlanetMatrix*(mVertex * uSkyRadius);
    gl_Position = position.xyww;
}

@fragment:
uniform float uExposure;
uniform vec3 uLightDirection;
uniform vec3 uInvWaveLength;
uniform float uKr4Pi;
uniform float uKm4Pi;
uniform float uKrESun;
uniform float uKmESun;
uniform float uSkyRadius;
uniform float uGroundRadius;
uniform float uScale;
uniform float uScaleDepth;

in vec3 vCameraDirection;
in vec3 vCameraPosition;
in float vDistance;

out vec4 fragColor;

const float g = -0.9999;
const float g2 = g*g;
const int noSamples = 5;
const float invSamples = 1.0/noSamples;

float scale(float fCos){
    float x = 1.0 - fCos;
    return uScaleDepth * exp(-0.00287 + x*(0.459 + x*(3.83 + x*(-6.80 + x*5.25))));
}
void atmosphere(in vec3 start, in vec3 ray, in float farDistance, inout vec3 rayleighColor, inout vec3 mieColor){
    float height = length(start);
    float scaleOverScaleDepth = uScale / uScaleDepth;
    float depth = exp(scaleOverScaleDepth * ( uGroundRadius - height));
    float startAngle = dot(ray, start) / height;
    float startOffset = depth*scale(startAngle);

    vec3 color = vec3(0);
    float sampleLength = farDistance * invSamples;
    float scaledLength = sampleLength * uScale;
    vec3 sampleRay = ray * sampleLength;
    vec3 samplePoint = start + sampleRay * 0.5;

    for(int i=0; i<noSamples; i++){
        height = length(samplePoint);
        depth = exp(scaleOverScaleDepth * (uGroundRadius - height));
        float lightAngle = dot(uLightDirection, samplePoint) / height;
        float cameraAngle = dot(ray, samplePoint) / height;
        float scatter = (startOffset + depth*(scale(lightAngle) - scale(cameraAngle)));

        vec3 attenuate = exp(-scatter * (uInvWaveLength * uKr4Pi + uKm4Pi));
        color += attenuate * (depth * scaledLength);
        samplePoint += sampleRay;
    }

    rayleighColor.xyz = color * uInvWaveLength * uKmESun;
    mieColor.xyz = color * uKrESun;
}

void main(){
    vec3 cameraRay = normalize(vCameraDirection);
    float fCos = dot(uLightDirection, -cameraRay);
    float rayleighPhase = 0.75 * (1.0 + fCos*fCos);
    float miePhase = 1.5 * ((1.0 - g2) / (2.0 + g2)) * (1.0 + fCos*fCos) / pow(1.0 + g2 - 2.0*g*fCos, 1.5);

    vec3 rayleighColor;
    vec3 mieColor;

    atmosphere(vCameraPosition, cameraRay, vDistance, rayleighColor, mieColor);

    // fragColor.rgb = 1.0 - exp( -uExposure/10 * (rayleighPhase*rayleighColor + miePhase*mieColor) );
    fragColor.rgb = 1.0 - exp( - 0.1*(rayleighPhase*rayleighColor + miePhase*mieColor) );
    fragColor.a = 1;
}
