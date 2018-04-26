#ifdef VERTEX_SHADER

layout(location=0)in vec4 mVertex;

out vec2 vUV;

void main(){
    vUV = mVertex.zw;
    gl_Position = (vec4(mVertex.xy,0,1));
}

#endif

#ifdef FRAGMENT_SHADER
out vec4 outColor;

uniform sampler2D uTexture;
uniform vec2 uPixelSize;

in vec2 vUV;

vec4 GaussianBlur(in sampler2D tex0, in vec2 centreUV, in vec2 pixelOffset){
    vec4 colOut = vec4(0);

    /// Kernel width 35 x 35
    const int stepCount = 9;
    const float gWeights[stepCount] = float[](
       0.10855,
       0.13135,
       0.10406,
       0.07216,
       0.04380,
       0.02328,
       0.01083,
       0.00441,
       0.00157
    );
    const float gOffsets[stepCount] = float[](
       0.66293,
       2.47904,
       4.46232,
       6.44568,
       8.42917,
       10.41281,
       12.39664,
       14.38070,
       16.36501
    );

    for( int i = 0; i < stepCount; i++ )
    {
        vec2 texCoordOffset = gOffsets[i] * pixelOffset;
        vec4 col = texture( tex0, centreUV + texCoordOffset ) + texture( tex0, centreUV - texCoordOffset );
        colOut += gWeights[i] * col;
    }

    return colOut;
}

void main(void){
    outColor = GaussianBlur(uTexture, vUV, uPixelSize/2*vec2(0,1));
}

#endif
