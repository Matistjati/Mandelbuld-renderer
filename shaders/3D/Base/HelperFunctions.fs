<boundingSphere>
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
</boundingSphere>

<sphereFold>
	void sphereFold(inout vec3 z, inout float dz, float r2, float innerRadius, float outerRadius)
	{
		if (r2 < innerRadius)
		{ 
			// linear inner scaling
			float temp = (outerRadius/innerRadius);
			z *= temp;
			dz*= temp;
		}
		else if (r2<outerRadius)
		{ 
			// this is the actual sphere inversion
			float temp =(outerRadius/r2);
			z *= temp;
			dz*= temp;
		}
	}
</sphereFold>

<boxFold>
	void boxFold(inout vec3 w, float foldingLimit)
	{
		w = clamp(w, -foldingLimit, foldingLimit) * 2.0 - w;
	}
</boxFold>

<triplexPow>
vec3 triplexPow(vec3 w, float power, inout float dw, float m)
{
	dw = (power * pow(sqrt(m), power - 1)) * dw + 1.0;

	float r = length(w);
	float theta = power * atan(w.x, w.z);
	float phi = power * acos(w.y / r);

	// Fun alternative: reverse sin and cos
	return pow(r, power) * vec3(sin(theta) * sin(phi), cos(phi), cos(theta) * sin(phi));
}
</triplexPow>