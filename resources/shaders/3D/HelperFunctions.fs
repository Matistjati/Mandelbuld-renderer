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
%/boundingSphere%