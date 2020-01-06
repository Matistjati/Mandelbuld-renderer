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
        // Sphere inversion in 3D
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

<sierpinskiFold>
	// Fold space about the symmetry planes of a tetrahedron
	void sierpinskiFold(inout vec3 w)
	{
		if(w.x + w.y < 0) w.xy = -w.yx;
		if(w.x + w.z < 0) w.xz = -w.zx;
		if(w.y + w.z < 0) w.yz = -w.zy;
	}
</sierpinskiFold>

<mengerFold>
	// Fold space about the symmetry planes of a menger sponge
	void mengerFold(inout vec3 w)
	{
		w=abs(w);
		if(w.x-w.y<0) w.xy = w.yx;
		if(w.x-w.z<0) w.xz = w.zx;
		if(w.y-w.z<0) w.yz = w.zy;
	}
</mengerFold>

<icosaFold>
	// Fold space about the symmetry planes of an icosahedron
	void icosaFold(inout vec3 w)
	{
		const float phi = 1.61803399; // golden ratio.

		const vec3 n1 = normalize(vec3(-phi,phi-1.0,1.0));
		const vec3 n2 = normalize(vec3(1.0,-phi,phi+1.0));

		w.yz=abs(w.yz);
		w-=2.0 * max(0,dot(w, n2)) * n2;

		w.xz = abs(w.xz);
		w-=2.0 * max(0,dot(w, n1)) * n1;
	}
</icosaFold>

<triplexPow>
vec3 triplexPow(vec3 w, float power, inout float dw, float m)
{
	dw = (power * pow(sqrt(m), power - 1)) * dw + 1.0;

	float r = length(w);
#if 1
	float theta = power * atan(w.x, w.z);
	float phi = power * acos(w.y / r);

	// Fun alternative: reverse sin and cos
	return pow(r, power) * vec3(sin(theta) * sin(phi), cos(phi), cos(theta) * sin(phi));
#else
	float theta = power * atan(w.y, w.x);
    float phi = power * asin(w.z / r);

	// Fun alternative: reverse sin and cos
	//return pow(r, power) * vec3(sin(theta) * sin(phi), cos(phi), cos(theta) * sin(phi));
	return pow(r, power) * vec3(cos(theta)*cos(phi), sin(theta)*cos(phi), sin(phi));
#endif
}
</triplexPow>
