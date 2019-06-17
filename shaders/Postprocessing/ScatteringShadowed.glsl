@vertex:
layout(location=0)in vec4 mVertex;

uniform vec4 uCameraVectors[5];

out vec4 vFrustumDirection;
out vec4 vCameraDirection;
out vec2 vUV;

void main(){
    vUV = mVertex.zw;
    vCameraDirection = uCameraVectors[4];
    vFrustumDirection = uCameraVectors[gl_VertexID];
    gl_Position = vec4(mVertex.xy, 1, 1);
}

/**
 *  nice work here:
 *  https://atomworld.wordpress.com/tag/shader/
 *  http://www.scratchapixel.com/old/lessons/3d-advanced-lessons/simulating-the-colors-of-the-sky/atmospheric-scattering/
 *  https://atomworld.files.wordpress.com/2014/12/sample_code_gss01.jpg
 *  maybe: http://www.iquilezles.org/www/articles/fog/fog.htm
*/

@fragment:

uniform float uNear;
uniform float uFar;
uniform vec4 uEyePosition;
uniform vec4 uLightDirection;
uniform mat4 uInvPV;

uniform sampler2D uDepth;
uniform sampler2D uColor;
uniform sampler2DArrayShadow uCSMCascades;
uniform vec2 uPixelSize;
uniform mat4 uCSMProjection[4];
uniform float uSplitDistances[4];

uniform vec4 uWaveLength;
uniform float uRayleigh;
uniform float uMie;
uniform float uRayleighSun;
uniform float uMieSun;
uniform float uSkyRadius;
uniform float uGroundRadius;
uniform float uScaleDepth;
uniform float uCameraHeight;

in vec4 vFrustumDirection;
in vec4 vCameraDirection;
in vec2 vUV;

out vec4 outColor;
const float g = -0.1999;
// const float g = -0.5998;
const float g2 = g*g;
const int samples = 30;
const float invSamples = 1.0/samples;
const float adjust = 1.0/100000.0;
const float fExposure = 0.2;

float calcShadow(in vec3 P, in int cascade){
    vec4 positionLS = (uCSMProjection[cascade]*vec4(P, 1))*0.5 + 0.5;
    vec2 UVCoords = positionLS.xy;

    if(UVCoords.x < 0 || UVCoords.x > 1 || UVCoords.y < 0 || UVCoords.y > 1 || positionLS.z > 1) return 1.0;

    float xOffset = 1.0/2048.0;
    float yOffset = 1.0/2048.0;

    float factor = 0.0;

    vec4 UVC = vec4(UVCoords, cascade, positionLS.z);
    factor += texture(uCSMCascades, UVC);
    factor = 0.4 + (factor / 1.0);

    return factor > 0.98 ? 1.0 : factor;
}

float isInShadow(in vec3 P){
    float depth = distance(P, uEyePosition.xyz);
    for(int i=1; i<4; i++){
        if(depth < uSplitDistances[i]){
            return calcShadow(P, i);
        }
    }
    return 1.0;
}

vec4 getPosition(){
    float depth = 2*texture(uDepth, vUV).r-1;
    vec4 viewSpace = vec4(vUV*2-1, depth, 1);
    vec4 worldPos = uInvPV*viewSpace;
    worldPos.xyzw /= worldPos.w;
    return worldPos;
}
float scaleFactor(float angle){
    float x = 1.0 - angle;
    return uScaleDepth * exp(-0.00287 + x*(0.459 + x*(3.83 + x*(-6.80 + x*5.25))));
}

void atmospheric(vec4 position, out vec4 rayleighColor, out vec4 mieColor, out vec4 cameraDirection){
    vec3 lightDir = -uLightDirection.xyz;
    vec3 farPosition = position.xyz;
    vec3 cameraPosition = uEyePosition.xyz;
    vec3 ray = farPosition - cameraPosition;
    float far = length(ray);
    ray /= far;

    float scale = 1.0 / (80000); /// 1/(1.5) = 0.66
    float scaleOverScaleDepth = scale/uScaleDepth;
    float depth = exp(-(scaleOverScaleDepth * 0.1)*adjust);
    float lightAngle = clamp(dot(lightDir, vec3(0,0,1)), -1,1);
    float lightScale = scaleFactor(lightAngle);
    float cameraAngle = abs(dot(ray, -vCameraDirection.xyz));

    float cameraScale = scaleFactor(cameraAngle); /// camera scale ma jaki� dziwny wplyw na wysoko��
    float cameraOffset = depth*cameraScale;
    float temp = (lightScale + cameraScale);

    // Initialize the scattering loop variables
    vec3 color = vec3(0);
    float sampleLength = far * invSamples;
    float scaledLength = sampleLength * scale;
    vec3 sampleRay = ray * sampleLength;
    vec3 samplePoint = cameraPosition + sampleRay * 0.5;

    vec3 totalAttenuate = vec3(0);
    float inShadow = 0;
    float total = 0.1;
    for(int i=0; i<samples; i++){
        // float height = samplePoint.z;
        float height = length(samplePoint);

        float shadow = isInShadow(samplePoint);
        inShadow += shadow * (samples - i);
        total += (samples - i);
        depth = exp(-(scaleOverScaleDepth * height)); /// or height/8300
        /// TODO: fix it
        float scatter = depth*(lightScale - cameraScale) + cameraOffset;
        // float scatter = depth*(lightScale);
        vec3 attenuate = exp(-scatter * (uWaveLength.xyz * uRayleigh + vec3(uMie)));
        color += attenuate * (depth * scaledLength);
        totalAttenuate += attenuate;
        samplePoint += sampleRay;
    }
    float v = pow(inShadow/total, 2);
    rayleighColor.xyz = color * (uWaveLength.xyz * uRayleighSun + uMieSun) * v;
    mieColor.xyz = totalAttenuate *v;
    // mieColor.xyz = color * uMieSun;
    cameraDirection = vec4(cameraPosition - farPosition, 0);
}

float getRayleighPhase(float lightAngle2){
    return 0.75 * (2.0 + 0.5 * lightAngle2);
}

vec4 finalColor(vec4 rayleighColor, vec4 mieColor, vec4 cameraDirection){
    vec4 color = texture(uColor, vUV);
    vec4 lightDir = -uLightDirection;
    // float lightAngle = clamp(dot(lightDir, normalize(cameraDirection)), -1, 1);
    float lightAngle = clamp(dot(lightDir, normalize(cameraDirection)), 0, 1);
    float lightAngle2 = lightAngle * lightAngle;

    float miePhase = 1.5 * ((1.0 - g2) / (2.0 + g2)) * (1.0 + lightAngle2) / pow(1.0 + g2 - 2.0*g*lightAngle, 1.5);
    float rayleighPhase = getRayleighPhase(lightAngle2);

    vec4 fragColor = 1-exp( -1 * (rayleighPhase*rayleighColor +  mieColor*miePhase*color) );


    // return color;
    // return rayleighColor;
    // return mieColor*color;
    // return vec4( miePhase/100 );
    return fragColor;
}

/// https://github.com/GameTechDev/OutdoorLightScattering

void main(){
    vec4 position = getPosition();
    vec4 rayleighColor;
    vec4 mieColor;
    vec4 cameraDirection;

    atmospheric(position, rayleighColor, mieColor, cameraDirection);

    outColor = finalColor(rayleighColor, mieColor, cameraDirection);
}
