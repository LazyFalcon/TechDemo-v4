@vertex:

layout(location=0)in vec4 mVertex;

void main(){
    gl_Position = (vec4(mVertex.xy,0,1));
}

@fragment:

out vec4 outColor;

uniform sampler2D uCombined;
@import: Uniforms

vec3 Uncharted2TonemapOperator(in vec3 x){
    const float A = 0.15;
    const float B = 0.50;
    const float C = 0.10;
    const float D = 0.20;
    const float E = 0.02;
    const float F = 0.30;

    return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}
vec3 Uncharted2Tonemap(in vec3 color){
    const float W = 1.2;

    color = Uncharted2TonemapOperator(color*uExposure*20);
    vec3 whiteScale = 1.0f/Uncharted2TonemapOperator(vec3(W));
    color *= whiteScale;

    return pow(color, vec3(1/uGamma));
}

void main(){
    outColor.rgb = texelFetch(uCombined, ivec2(gl_FragCoord.xy), 0).rgb;

    outColor.rgb = Uncharted2Tonemap(outColor.rgb);

    outColor.a = 1;
}
