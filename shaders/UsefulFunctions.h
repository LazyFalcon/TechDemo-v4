
vec3 getPosition(in vec2 uv){
    float depth = 2*texture(uDepth, uv).r-1;
    vec4 viewSpace = vec4(uv*2-1, depth, 1);
    vec4 worldPos = uInvPV*viewSpace;
    worldPos.xyzw /= worldPos.w;
    return worldPos.xyz;
}

vec4 getNormal(in vec2 uv){
    return texture(uNormals, uv);
}

float invSquareIntensity(in float x, in float r){
    float att = 1 / (1 + 0*x + 100*x*x) * max(0, 1-(x)/(r)) * max(0, 1-(x)/(r));
    return att;
}

vec4 vecPointToSegment(in vec3 point, in vec3 from, in vec3 to, in vec3 dir){
    float t = dot(point-from, dir);
    float tClamped = clamp(t, 0, distance(from, to));
    return vec4((point-from) - tClamped*dir, t != tClamped);
}

vec3 closestToRay(in vec3 rayRef, in vec3 ray, in vec3 viewRef, in vec3 view){
    vec3 ortho = cross(ray, view);
    vec3 plane = cross(ray, ortho);
    return viewRef + dot(plane, rayRef-viewRef) / dot(plane, view) * view;
}
