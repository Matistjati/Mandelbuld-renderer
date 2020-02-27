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

	float hash13(vec3 p)
	{
		p  = fract(p*0.3183099+.1);
		p *= 17.0;
		return fract(p.x*p.y*p.z*(p.x+p.y+p.z));
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

	float smoothSample(in vec3 x)
	{
		vec3 i = floor(x);
		vec3 f = fract(x);
		f = f*f*(3.0-2.0*f);
	
		return mix(mix(mix(hash13(i+vec3(0,0,0)), 
						   hash13(i+vec3(1,0,0)),f.x),
					   mix(hash13(i+vec3(0,1,0)), 
						   hash13(i+vec3(1,1,0)),f.x),f.y),
				   mix(mix(hash13(i+vec3(0,0,1)), 
						   hash13(i+vec3(1,0,1)),f.x),
					   mix(hash13(i+vec3(0,1,1)), 
						   hash13(i+vec3(1,1,1)),f.x),f.y),f.z);
	}

	const mat3 m = mat3( 0.00,  0.80,  0.60,
						-0.80,  0.36, -0.48,
						-0.60, -0.48,  0.64 );

	float fbm(vec3 p)
	{
		vec3 q = p;
		float f;
		f  = 0.50000*smoothSample(q); q = m*q*2.02;
		f += 0.25000*smoothSample(q); q = m*q*2.03;
		f += 0.12500*smoothSample(q); q = m*q*2.01;
		f += 0.06250*smoothSample(q); q = m*q*2.02;
		f += 0.03125*smoothSample(q);
		return clamp(f-0.5, 0.0, 1.0 );
	}
</random>

<remap>
	float remap(float value, float lowOrig, float highOrig, float lowNew, float highNew)
	{
		return lowNew + (value - lowOrig) * (highNew - lowNew) / (highOrig - lowOrig);
	}
</remap>

<boundingGeometry>
	vec2 RayBoxIntersection(vec3 rayOrigin, vec3 rayDir, vec3 boxMin, vec3 boxMax)
	{
		vec3 tMin = (boxMin - rayOrigin) / rayDir;
		vec3 tMax = (boxMax - rayOrigin) / rayDir;
		vec3 t1 = min(tMin, tMax);
		vec3 t2 = max(tMin, tMax);
		float tNear = max(max(t1.x, t1.y), t1.z);
		float tFar = min(min(t2.x, t2.y), t2.z);
		return vec2(tNear, tFar);
	}

	vec2 RaySphereIntersection(vec4 sph, vec3 ro, vec3 rd)
	{
		vec3 oc = ro - sph.xyz;
	
		float b = dot(oc,rd);
		float c = dot(oc,oc) - sph.w*sph.w;
		float h = b*b - c;
	
		if( h<0.0 ) return vec2(-1.0);

		h = sqrt( h );

		return -b + vec2(-h,h);
	}
</boundingGeometry>

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

<pathTracing>
	vec3 cosineDirection( in float seed, in vec3 nor)
	{
		float u = hash11(78.233 + seed);
		float v = hash11(10.873 + seed);

		// Method 1 and 2 first generate a frame of reference to use with an arbitrary
		// distribution, cosine in this case. Method 3 (invented by fizzer) specializes 
		// the whole math to the cosine distribution and simplfies the result to a more 
		// compact version that does not depend on a full frame of reference.

		#if 0
			// method 1 by http://orbit.dtu.dk/fedora/objects/orbit:113874/datastreams/file_75b66578-222e-4c7d-abdf-f7e255100209/content
			vec3 tc = vec3( 1.0+nor.z-nor.xy*nor.xy, -nor.x*nor.y)/(1.0+nor.z);
			vec3 uu = vec3( tc.x, tc.z, -nor.x );
			vec3 vv = vec3( tc.z, tc.y, -nor.y );

			float a = 6.2831853 * v;
			return sqrt(u)*(cos(a)*uu + sin(a)*vv) + sqrt(1.0-u)*nor;
		#endif
		#if 0
			// method 2 by pixar:  http://jcgt.org/published/0006/01/01/paper.pdf
			float ks = (nor.z>=0.0)?1.0:-1.0;     //do not use sign(nor.z), it can produce 0.0
			float ka = 1.0 / (1.0 + abs(nor.z));
			float kb = -ks * nor.x * nor.y * ka;
			vec3 uu = vec3(1.0 - nor.x * nor.x * ka, ks*kb, -ks*nor.x);
			vec3 vv = vec3(kb, ks - nor.y * nor.y * ka * ks, -nor.y);
	
			float a = 6.2831853 * v;
			return sqrt(u)*(cos(a)*uu + sin(a)*vv) + sqrt(1.0-u)*nor;
		#endif
		#if 1
			// method 3 by fizzer: http://www.amietia.com/lambertnotangent.html
			float a = 6.2831853 * v;
			u = 2.0*u - 1.0;
			return normalize( nor + vec3(sqrt(1.0-u*u) * vec2(cos(a), sin(a)), u) );
		#endif
	}

	vec3 uniformVector(float seed)
	{
		float a = 3.141593*hash11(78.233 + seed);
		float b = 6.283185*hash11(10.873 + seed);
		return vec3( sin(b)*sin(a), cos(b)*sin(a), cos(a) );
	}
</pathTracing>

<clouds>
	float cloudNoise(vec3 pos)
	{
		float baseShape = fbm(pos*noiseScale.x);

		float heightPercent = (boxPos.y+boxWidth.y*0.5-pos.y)/(boxWidth.y);
		// https://www.desmos.com/calculator/st2nn7aos1
		// Remove smooth out bottom (approximately, lost original curve hehe)
		baseShape *= 1+(-1)/(1.3+pow(heightPercent/0.87,-80));
		// Smooth out top
		baseShape *= heightPercent * heightWeight;

		// Smooth out cloud edges
		// Simplify (boxPos.y+boxWidth.y*0.5 - pos.y)/(boxPos.y+boxWidth*0.5-(boxPos.y-boxWidth*0.5)) to arrive at the following:
		vec2 posPercent = abs(boxPos.xz-pos.xz)/(boxWidth.xz*0.5);
		float minPos = 1-max(posPercent.x, posPercent.y);
		// https://www.desmos.com/calculator/s0gl9g0f6p
		// Multiply by following (approximately, lost original curve hehe)
		baseShape *= edgeDensity * (1.000014 + (0.2 - 1.000014)/(1 + pow(minPos/0.1414233, 5.614542)));

		if (baseShape > 0)
		{
			vec3 detailSamplePos = pos;
			vec3 detailNoise;
			detailNoise.x = 1-cellular(detailSamplePos, detailNoiseScale.x);
			detailNoise.y = 1-cellular(detailSamplePos, detailNoiseScale.y);
			detailNoise.z = 1-cellular(detailSamplePos, detailNoiseScale.z);
			float detailFBM = dot(detailNoise, detailWeights);

			// Subtract detail noise from base shape (weighted by inverse density so that edges get eroded more than centre)
			float oneMinusShape = 1 - baseShape;
			float detailErodeWeight = oneMinusShape * oneMinusShape * oneMinusShape;
			float cloudDensity = baseShape - (1-detailFBM) * detailErodeWeight * detailNoiseWeight;

			return max(0, cloudDensity - densityThreshold) * densityLevel;
		}
		return 0;
	}

	#define boxMin boxPos-boxWidth*0.5
	#define boxMax boxPos+boxWidth*0.5
	float lightMarch(vec3 pos)
	{
		float distInsidebox = RayBoxIntersection(pos, -sun, boxMin, boxMax).y;

		float sunStepSize = distInsidebox/stepsToLight;
		float totalDensity = 0;

		for (int step = 0; step < stepsToLight; step++)
		{
			pos += -(sun) * sunStepSize;
			totalDensity += max(0, cloudNoise(pos)) * sunStepSize;
		}

		float transmittance = exp(-totalDensity * lightAbsorbtionSun);
		return darknessThreshold + transmittance * (1-darknessThreshold);
	}

	float hg(float a, float g)
	{
        float g2 = g*g;
        return (1-g2) / (4*3.1415*pow(1+g2-2*g*(a), 1.5));
    }

	float phase(float a)
	{
        float blend = .5;
        float hgBlend = hg(a,phaseParams.x) * (1-blend) + hg(a,-phaseParams.y) * blend;
        return phaseParams.z + hgBlend*phaseParams.w;
    }

	float SampleCloudDensity(vec3 ro, vec3 rd, out vec3 cloudCol)
	{
		vec2 intersect = RayBoxIntersection(ro, rd, boxMin, boxMax);
		if (intersect.y<0)
		{
			return 1;
		}

		float cosAngle = dot(rd, -(sun));
        float phaseVal = phase(cosAngle);

		float transmittance = 1;
		vec3 lightEnergy = vec3(0);
		float delta = max(0.05, stepSize);
		for (float t = intersect.x; t < intersect.y; t+=delta)
		{
			float density = cloudNoise(ro+rd*t) * delta;
			if (density > 0)
			{
				transmittance *= exp(-density * delta * lightAbsorptionThroughCloud);
				float lightTransmittance = lightMarch(ro+rd*t);
				lightEnergy += density * delta * transmittance * lightTransmittance * phaseVal;

				// Early exit
				if (transmittance < 0.01)
				{
					break;
				}
				// Step further when deeper into the volume
				delta = max(0.05, stepSize*t);
			}
		}

		cloudCol = lightEnergy*sunColor*cloudBrightness;

		return transmittance;
	}
</clouds>