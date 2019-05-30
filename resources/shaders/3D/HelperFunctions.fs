%boundingSphere%
vec2 boundingSphere(vec4 sph, vec3 origin, vec3 ray)
{
    vec3 oc = origin - sph.xyz;
    
	float b = dot(oc,ray);
	float c = dot(oc,oc) - sph.w*sph.w;
    float h = b*b - c;
    
    if( h<0.0 ) return vec2(-1.0);

    h = sqrt( h );

    return -b + vec2(-h,h);
}
%boundingSphere%

%calculateNormal%
vec3 calculateNormal(vec3 p)
{
    const vec3 small_step = vec3(0.001, 0.0, 0.0);

	vec3 gradient;
	vec4 temp;

    gradient.x = sceneDistance(p + small_step.xyy, temp) - sceneDistance(p - small_step.xyy, temp);
    gradient.y = sceneDistance(p + small_step.yxy, temp) - sceneDistance(p - small_step.yxy, temp);
    gradient.z = sceneDistance(p + small_step.yyx, temp) - sceneDistance(p - small_step.yyx, temp);

    return normalize(gradient);
}
%/calculateNormal%

%softShadow%
float softShadow(Ray ray, float k)
{
    float result = 1.0;
    float t = 0.0;
	
	vec4 temp;

    for(int i = 0; i < maxSteps; i++)
    {
        float h = sceneDistance(ray.origin + ray.dir * t, temp);
        result = min(result, k * h / t);

        if(result < 0.001) break;

        t += clamp(h, 0.01, 32);
    }
    return clamp(result, 0.0, 1.0);
}
%/softShadow%