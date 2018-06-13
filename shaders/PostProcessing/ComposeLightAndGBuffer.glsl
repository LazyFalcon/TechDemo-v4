#ifdef VERTEX_SHADER

layout(location=0)in vec4 mVertex;

out vec2 vUV;

void main(){
    gl_Position = (vec4(mVertex.xy,1,1));
    vUV = mVertex.zw;
}

#endif

#ifdef FRAGMENT_SHADER

out vec4 outColor;

in vec2 vUV;

uniform sampler2D uColor;
uniform sampler2D uLight;
uniform sampler2D uSpecular;
uniform sampler2D uAO;

uniform mat4 uInvPV;
uniform vec3 uEye;
uniform vec3 uViewDir;
uniform vec2 uPixelSize;

uniform float uGamma;

void main(void){
    vec2 uv = gl_FragCoord.xy * uPixelSize;

    vec3 albedo = pow(texture(uColor, vUV).rgb, vec3(uGamma));
    vec3 light = texture(uLight, vUV).rgb;
    vec3 specular = texture(uSpecular, vUV).rgb;
    float ao = texture(uAO, vUV).r;

    // vec3 final = albedo*light*ao + specular;

    vec3 final = albedo*ao + (1-ao)*vec3(20,10,5)/256;
    // final = light;
    // final = specular;
    // final = albedo;
    outColor = vec4(final, 1);
}

#endif
