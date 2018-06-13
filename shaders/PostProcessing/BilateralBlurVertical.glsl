#ifdef VERTEX_SHADER

layout(location=0)in vec4 mVertex;

out vec2 vUV;

void main(){
    gl_Position = (vec4(mVertex.xy,0,1));
    vUV = (mVertex.xy+vec2(1,1))/2;
}


#endif

#ifdef FRAGMENT_SHADER
out vec2 outAODepth;

uniform sampler2D uTexture;
uniform float uBlurSize;
uniform vec2 uPixelSize;

#define SIGMA 10.0
#define BSIGMA 0.2
#define MSIZE 8

in vec2 vUV;

float normpdf(in float x, in float sigma){
    return 0.39894*exp(-0.5*x*x/(sigma*sigma))/sigma;
}

float normpdf3(in vec2 v, in float sigma){
    return 0.39894*exp(-0.5*dot(v,v)/(sigma*sigma))/sigma;
}
// float normpdf3(in float v, in float sigma){
//     return 0.39894*exp(-0.5*dot(v,v)/(sigma*sigma))/sigma;
// }
#define LAYER_TO_SAMPLE 0
void main(void){
    outAODepth = vec2(0);
    ivec2 position = ivec2(gl_FragCoord.xy);
    vec2 center = texelFetch(uTexture, position, LAYER_TO_SAMPLE).rg;
    const int kernelSize = (MSIZE-1)/2;
    float kernel[MSIZE];

    float Z = 0.0;
    for (int j = 0; j <= kernelSize; ++j){
        kernel[kernelSize+j] = kernel[kernelSize-j] = normpdf(float(j), SIGMA);
    }

    vec2 s;
    float factor;
    float bZ = 1.0/normpdf(0.0, BSIGMA);
    ivec2 maxCoord = textureSize(uTexture, LAYER_TO_SAMPLE) - ivec2(1);
    //read out the texels
    for(int j=-kernelSize; j <= kernelSize; ++j){
        s = texelFetch(uTexture, clamp(position + ivec2(0, j), ivec2(0), maxCoord), LAYER_TO_SAMPLE).rg;
        factor = normpdf3(s-center, BSIGMA)*bZ*kernel[kernelSize-j];
        Z += factor;
        outAODepth.r += factor*s.r;
    }
    outAODepth.r /= Z;
    outAODepth = vec2(outAODepth.r);
}

#endif
