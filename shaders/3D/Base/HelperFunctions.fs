<random>
	vec2 UniformToNormal(vec2 randuniform)
	{
		vec2 r = randuniform;
		r.x = sqrt(-2.*log(1e-9+abs(r.x)));
		r.y *= 6.28318;
		r = r.x*vec2(cos(r.y),sin(r.y));
		return r;
	}

	float hash11(float p)
	{
		p = fract(p * .1031);
		p *= p + 33.33;
		p *= p + p;
		return abs(fract(p));
	}

	vec2 hash21(inout float p)
	{
		p = hash11(p);
		p = hash11(p);
		vec3 p3 = fract(vec3(p) * vec3(.1031, .1030, .0973));
		p3 += dot(p3, p3.yzx + 33.33);
		return fract((p3.xx+p3.yz)*p3.zy);
	}

	vec2 hash22(vec2 p)
	{
		vec3 p3 = fract(vec3(p.xyx) * vec3(.1031, .1030, .0973));
		p3 += dot(p3, p3.yzx+33.33);
		return fract((p3.xx+p3.yz)*p3.zy);
	}

	vec2 hash23(vec3 p)
	{
		vec4 p4 = fract(vec4(p.xyzx)  * vec4(.1031, .1030, .0973, .1099));
		p4 += dot(p4, p4.wzxy+33.33);
		return (fract((p4.xxyz+p4.yzzw)*p4.zywx)).xy;
	}

	uint intHash(uint x)
	{
		x = ((x >> 16) ^ x) * 0x45d9f3bU;
		x = ((x >> 16) ^ x) * 0x45d9f3bU;
		x = (x >> 16) ^ x;
		return x;
	}

	vec2 hash2(uint n, out uint hash)
	{
		uint ih =intHash(n);
		hash = intHash(ih);
		uvec2 k = uvec2(ih,hash);
		return vec2(k & uvec2(0xffffffffU))/float(0xffffffffU);
	}

	// Cellular noise taken from: https://github.com/ashima/webgl-noise

	//Copyright (C) 2011 by Ashima Arts (Simplex noise)
	//Copyright (C) 2011-2016 by Stefan Gustavson (Classic noise and others)

	//Permission is hereby granted, free of charge, to any person obtaining a copy
	//of this software and associated documentation files (the "Software"), to deal
	//in the Software without restriction, including without limitation the rights
	//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	//copies of the Software, and to permit persons to whom the Software is
	//furnished to do so, subject to the following conditions:

	//The above copyright notice and this permission notice shall be included in
	//all copies or substantial portions of the Software.

	//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	//THE SOFTWARE.

	vec4 mod289(vec4 x)
	{
	  return x - floor(x * (1.0 / 289.0)) * 289.0;
	}

	vec3 mod289(vec3 x)
	{
	  return x - floor(x * (1.0 / 289.0)) * 289.0;
	}

	vec2 mod289(vec2 x)
	{
	  return x - floor(x * (1.0 / 289.0)) * 289.0;
	}

	// Modulo 7 without a division
	vec3 mod7(vec3 x)
	{
	  return x - floor(x * (1.0 / 7.0)) * 7.0;
	}

	// Modulo 7 without a division
	vec4 mod7(vec4 x)
	{
	  return x - floor(x * (1.0 / 7.0)) * 7.0;
	}

	// Permutation polynomial: (34x^2 + x) mod 289
	vec3 permute(vec3 x)
	{
	  return mod289((34.0 * x + 1.0) * x);
	}

	vec4 permute(vec4 x)
	{
	  return mod289((34.0 * x + 1.0) * x);
	}

	vec4 taylorInvSqrt(vec4 r)
	{
	  return 1.79284291400159 - 0.85373472095314 * r;
	}

	// Cellular noise, returning F1 and F2 in a vec2.
	// Standard 3x3 search window for good F1 and F2 values
	float cellular(vec2 P)
	{
		#define K 0.142857142857 // 1/7
		#define Ko 0.428571428571 // 3/7
		const float jitter = 1.0; // Less gives more regular pattern
		P.x*=3;
		P.y*=5;
		vec2 Pi = mod289(floor(P));
		vec2 Pf = fract(P);
		vec3 oi = vec3(-1.0, 0.0, 1.0);
		vec3 of = vec3(-0.5, 0.5, 1.5);
		vec3 px = permute(Pi.x + oi);
		vec3 p = permute(px.x + Pi.y + oi); // p11, p12, p13
		vec3 ox = fract(p*K) - Ko;
		vec3 oy = mod7(floor(p*K))*K - Ko;
		vec3 dx = Pf.x + 0.5 + jitter*ox;
		vec3 dy = Pf.y - of + jitter*oy;
		vec3 d1 = dx * dx + dy * dy; // d11, d12 and d13, squared
		p = permute(px.y + Pi.y + oi); // p21, p22, p23
		ox = fract(p*K) - Ko;
		oy = mod7(floor(p*K))*K - Ko;
		dx = Pf.x - 0.5 + jitter*ox;
		dy = Pf.y - of + jitter*oy;
		vec3 d2 = dx * dx + dy * dy; // d21, d22 and d23, squared
		p = permute(px.z + Pi.y + oi); // p31, p32, p33
		ox = fract(p*K) - Ko;
		oy = mod7(floor(p*K))*K - Ko;
		dx = Pf.x - 1.5 + jitter*ox;
		dy = Pf.y - of + jitter*oy;
		vec3 d3 = dx * dx + dy * dy; // d31, d32 and d33, squared
		// Sort out the two smallest distances (F1, F2)
		vec3 d1a = min(d1, d2);
		d2 = max(d1, d2); // Swap to keep candidates for F2
		d2 = min(d2, d3); // neither F1 nor F2 are now in d3
		d1 = min(d1a, d2); // F1 is now in d1
		d2 = max(d1a, d2); // Swap to keep candidates for F2
		d1.xy = (d1.x < d1.y) ? d1.xy : d1.yx; // Swap if smaller
		d1.xz = (d1.x < d1.z) ? d1.xz : d1.zx; // F1 is in d1.x
		d1.yz = min(d1.yz, d2.yz); // F2 is now not in d2.yz
		d1.y = min(d1.y, d1.z); // nor in  d1.z
		d1.y = min(d1.y, d2.x); // F2 is in d1.y, we're done.
		return sqrt(d1.y);
	}



	// Cellular noise, returning F1 and F2 in a vec2.
	// 3x3x3 search region for good F2 everywhere, but a lot
	// slower than the 2x2x2 version.
	// The code below is a bit scary even to its author,
	// but it has at least half decent performance on a
	// modern GPU. In any case, it beats any software
	// implementation of Worley noise hands down.
	float cellular(vec3 P, float scale)
	{
		#define K 0.142857142857 // 1/7
		#define Ko 0.428571428571 // 1/2-K/2
		#define K2 0.020408163265306 // 1/(7*7)
		#define Kz 0.166666666667 // 1/6
		#define Kzo 0.416666666667 // 1/2-1/6*2

		P*=scale;
		const float jitter = 0.8; // smaller jitter gives less errors in F2
		vec3 Pi = mod289(floor(P));
		vec3 Pf = fract(P);
		vec4 Pfx = Pf.x + vec4(0.0, -1.0, 0.0, -1.0);
		vec4 Pfy = Pf.y + vec4(0.0, 0.0, -1.0, -1.0);
		vec4 p = permute(Pi.x + vec4(0.0, 1.0, 0.0, 1.0));
		p = permute(p + Pi.y + vec4(0.0, 0.0, 1.0, 1.0));
		vec4 p1 = permute(p + Pi.z); // z+0
		vec4 p2 = permute(p + Pi.z + vec4(1.0)); // z+1
		vec4 ox1 = fract(p1*K) - Ko;
		vec4 oy1 = mod7(floor(p1*K))*K - Ko;
		vec4 oz1 = floor(p1*K2)*Kz - Kzo; // p1 < 289 guaranteed
		vec4 ox2 = fract(p2*K) - Ko;
		vec4 oy2 = mod7(floor(p2*K))*K - Ko;
		vec4 oz2 = floor(p2*K2)*Kz - Kzo;
		vec4 dx1 = Pfx + jitter*ox1;
		vec4 dy1 = Pfy + jitter*oy1;
		vec4 dz1 = Pf.z + jitter*oz1;
		vec4 dx2 = Pfx + jitter*ox2;
		vec4 dy2 = Pfy + jitter*oy2;
		vec4 dz2 = Pf.z - 1.0 + jitter*oz2;
		vec4 d1 = dx1 * dx1 + dy1 * dy1 + dz1 * dz1; // z+0
		vec4 d2 = dx2 * dx2 + dy2 * dy2 + dz2 * dz2; // z+1

		// Cheat and sort out only F1
		d1 = min(d1, d2);
		d1.xy = min(d1.xy, d1.wz);
		d1.x = min(d1.x, d1.y);
		return sqrt(d1.x);
	}

	float snoise(vec2 v)
	{
		const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
		0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
		-0.577350269189626,  // -1.0 + 2.0 * C.x
		0.024390243902439); // 1.0 / 41.0
		// First corner
		vec2 i  = floor(v + dot(v, C.yy) );
		vec2 x0 = v -   i + dot(i, C.xx);

		// Other corners
		vec2 i1;
		//i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0
		//i1.y = 1.0 - i1.x;
		i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
		// x0 = x0 - 0.0 + 0.0 * C.xx ;
		// x1 = x0 - i1 + 1.0 * C.xx ;
		// x2 = x0 - 1.0 + 2.0 * C.xx ;
		vec4 x12 = x0.xyxy + C.xxzz;
		x12.xy -= i1;

		// Permutations
		i = mod289(i); // Avoid truncation effects in permutation
		vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
		+ i.x + vec3(0.0, i1.x, 1.0 ));

		vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
		m = m*m ;
		m = m*m ;

		// Gradients: 41 points uniformly over a line, mapped onto a diamond.
		// The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)

		vec3 x = 2.0 * fract(p * C.www) - 1.0;
		vec3 h = abs(x) - 0.5;
		vec3 ox = floor(x + 0.5);
		vec3 a0 = x - ox;

		// Normalise gradients implicitly by scaling m
		// Approximation of: m *= inversesqrt( a0*a0 + h*h );
		m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );

		// Compute final noise value at P
		vec3 g;
		g.x  = a0.x  * x0.x  + h.x  * x0.y;
		g.yz = a0.yz * x12.xz + h.yz * x12.yw;
		return 130.0 * dot(m, g);
	}

	float snoise(vec3 v)
	{ 
		const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
		const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

		// First corner
		vec3 i  = floor(v + dot(v, C.yyy) );
		vec3 x0 =   v - i + dot(i, C.xxx) ;

		// Other corners
		vec3 g = step(x0.yzx, x0.xyz);
		vec3 l = 1.0 - g;
		vec3 i1 = min( g.xyz, l.zxy );
		vec3 i2 = max( g.xyz, l.zxy );

		//   x0 = x0 - 0.0 + 0.0 * C.xxx;
		//   x1 = x0 - i1  + 1.0 * C.xxx;
		//   x2 = x0 - i2  + 2.0 * C.xxx;
		//   x3 = x0 - 1.0 + 3.0 * C.xxx;
		vec3 x1 = x0 - i1 + C.xxx;
		vec3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
		vec3 x3 = x0 - D.yyy;      // -1.0+3.0*C.x = -0.5 = -D.y

		// Permutations
		i = mod289(i); 
		vec4 p = permute( permute( permute( 
				i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
			+ i.y + vec4(0.0, i1.y, i2.y, 1.0 )) 
			+ i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

		// Gradients: 7x7 points over a square, mapped onto an octahedron.
		// The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
		float n_ = 0.142857142857; // 1.0/7.0
		vec3  ns = n_ * D.wyz - D.xzx;

		vec4 j = p - 49.0 * floor(p * ns.z * ns.z);  //  mod(p,7*7)

		vec4 x_ = floor(j * ns.z);
		vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

		vec4 x = x_ *ns.x + ns.yyyy;
		vec4 y = y_ *ns.x + ns.yyyy;
		vec4 h = 1.0 - abs(x) - abs(y);

		vec4 b0 = vec4( x.xy, y.xy );
		vec4 b1 = vec4( x.zw, y.zw );

		//vec4 s0 = vec4(lessThan(b0,0.0))*2.0 - 1.0;
		//vec4 s1 = vec4(lessThan(b1,0.0))*2.0 - 1.0;
		vec4 s0 = floor(b0)*2.0 + 1.0;
		vec4 s1 = floor(b1)*2.0 + 1.0;
		vec4 sh = -step(h, vec4(0.0));

		vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
		vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

		vec3 p0 = vec3(a0.xy,h.x);
		vec3 p1 = vec3(a0.zw,h.y);
		vec3 p2 = vec3(a1.xy,h.z);
		vec3 p3 = vec3(a1.zw,h.w);

		//Normalise gradients
		vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
		p0 *= norm.x;
		p1 *= norm.y;
		p2 *= norm.z;
		p3 *= norm.w;

		// Mix final noise value
		vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
		m = m * m;
		return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1), 
								dot(p2,x2), dot(p3,x3) ) );
	}

	float hash13(vec3 p)  // replace this by something better
{
    p  = fract( p*0.3183099+.1 );
	p *= 17.0;
    return fract( p.x*p.y*p.z*(p.x+p.y+p.z) );
}

float N( in vec3 x )
{
    vec3 i = floor(x);
    vec3 f = fract(x);
    f = f*f*(3.0-2.0*f);
	
    return mix(mix(mix( hash13(i+vec3(0,0,0)), 
                        hash13(i+vec3(1,0,0)),f.x),
                   mix( hash13(i+vec3(0,1,0)), 
                        hash13(i+vec3(1,1,0)),f.x),f.y),
               mix(mix( hash13(i+vec3(0,0,1)), 
                        hash13(i+vec3(1,0,1)),f.x),
                   mix( hash13(i+vec3(0,1,1)), 
                        hash13(i+vec3(1,1,1)),f.x),f.y),f.z);
}

float map5( in vec3 p )
{
	vec3 q = p;
	float f;
    f  = 0.50000*N( q ); q = q*2.02;
    f += 0.25000*N( q ); q = q*2.03;
    f += 0.12500*N( q ); q = q*2.01;
    f += 0.06250*N( q ); q = q*2.02;
    f += 0.03125*N( q );
	return clamp(f-0.5, 0.0, 1.0 );
}
</random>

<remap>
	float remap(float value, float lowOrig, float highOrig, float lowNew, float highNew)
	{
		return lowNew + (value - lowOrig) * (highNew - lowNew) / (highOrig - lowOrig);
	}
</remap>

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
	// Sphere inversion
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
