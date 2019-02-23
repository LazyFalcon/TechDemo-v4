@vertex:

layout(location=0)in vec4 mVertex;

out vec2 vUV;

void main(){
    gl_Position = (vec4(mVertex.xy,1,1));
    vUV = mVertex.zw;
}

@fragment:
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
    // vec3 albedo = texture(uColor, vUV).rgb;
    vec3 light = texture(uLight, vUV).rgb;
    vec3 specular = texture(uSpecular, vUV).rgb;
    float ao = texture(uAO, vUV).r;

    vec3 final = albedo*light*ao + specular*ao + (1-min(ao, 1))*vec3(20,10,5)/512*0;
    // final = (light + specular)*ao;

    // vec3 final = light;
    // final = light;
    // final = specular;
    // final = albedo;
    // final = vec3(texture(uColor, vUV).a);
    // final = vec3(ao);
    outColor = vec4(final, 1);
}
