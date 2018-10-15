@vertex:

layout(location=0)in vec4 mVertex;

// TODO: revork to have from -> to. For flexibility
uniform vec4 pxCopyPolygon;
uniform vec2 pxViewSize;

void main(){
    vec2 cornerPosition = pxCopyPolygon.xy + pxCopyPolygon.zw * mVertex.xy;

    // transform to range [0,1] then to [-1, 1]
    cornerPosition = (cornerPosition / pxViewSize)*2 - vec2(1);

    gl_Position = vec4(cornerPosition, 0, 1);
}



@fragment:
out vec4 outColor;

uniform vec2 pxViewSize;
uniform sampler2D uTexture;

void main(){
    outColor = texture(uTexture, gl_FragCoord.xy/pxViewSize);
}
