<complexSquare>
	vec2 complexSquare(vec2 z)
	{
		return mat2(z, -z.y, z.x) * z;
	}
</complexSquare>

<complexPow>
	// Complex exponentiation- see https://en.wikipedia.org/wiki/Complex_number#Integer_and_fractional_exponents
	vec2 complexPow(vec2 z, float power)
	{
		float arg = atan(z.y,z.x) * power;
		return pow(length(z), power) * vec2(cos(arg), sin(arg));
	}
</complexPow>

<complexTan>
	vec2 complexTan(vec2 w)
	{
		vec2 w2 = w * 2;

		float denominator = cos(w2.x)+cosh(w2.y);

		return vec2(sin(w2.x)/denominator, sinh(w2.y)/denominator);
	}
</complexTan>

<complexSin>
	vec2 complexSin(vec2 w)
	{
		return vec2(sin(w.x)*cosh(w.y), cos(w.x)*sinh(w.y));
	}
</complexSin>

<complexCos>
	vec2 complexCos(vec2 w)
	{
		return vec2(cos(w.x)*cosh(w.y), -sin(w.x)*sinh(w.y));
	}
</complexCos>

<hash>
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
</hash>

<MandelbrotInteriorCheck>
	/*reference: https://en.wikipedia.org/wiki/Mandelbrot_set#Optimizations */
	bool notInMainCardioid(vec2 z)
	{
		vec2 c=z-vec2(0.25,0);
		float q = dot(c,c);
		return q*(q+(c.x))>0.25*z.y*z.y;
	}

	bool InMainCardioid(vec2 z)
	{
		vec2 c=z-vec2(0.25,0);
		float q = dot(c,c);
		return q*(q+(c.x))<0.25*z.y*z.y;
	}

	bool notInMainBulb(vec2 z)
	{
		z += vec2(1,0);
		return bool(step(0.062499999,dot(z,z)));
	}

	bool InMainBulb(vec2 z)
	{
		z += vec2(1,0);
		return bool(step(dot(z,z),0.062499999));
	}
</MandelbrotInteriorCheck>

<map01ToInterval>
vec2 map01ToInterval(vec2 value, vec4 range)
{
	return value*(range.zw-range.xy)+range.xy;
}

float map01ToInterval(float value, vec2 range)
{
	return value*(range.x-range.y)+range.x;
}
</map01ToInterval>

<polynomial>
#define cDiv(a,b) mat2(b.x,-b.y,b.y,b.x) * a / dot(b,b)
#define cMul(a,b) mat2(a,-a.y,a.x) * b

vec2 FindRoot(vec2 poly[size], int degree, uint seed)
{
	uint hash = intHash(seed+intHash(abs(int(frame))+degree*233960+intHash(gl_GlobalInvocationID.x))*intHash(gl_GlobalInvocationID.y));
	vec2 root = hash2(hash, hash);
	root = root.x * vec2(cos(root.y*6.28318530718), sin(root.y*6.28318530718));
	vec2 derivative[size];
	for (int i = 0; i < degree; i++)
	{
		derivative[i] = poly[i] * (degree-i);
	}

	for (int i = 0; i < maxIterations; i++)
	{
		vec2 dy = derivative[0];
		for (int i = 1; i < degree; i++)
		{
			dy = mat2(dy,-dy.y,dy.x) * root + derivative[i];
		}


		vec2 y = poly[0];
		for (int i = 1; i < degree+1; i++)
		{
			y = mat2(y,-y.y,y.x) * root + poly[i];
		}
		vec2 delta = mat2(dy.x,-dy.y,dy.y,dy.x)* y / dot(dy,dy);

		if (dot(delta,delta) < epsilon*epsilon)
		{
			break;
		}
		root -= delta;
	}

	return root;
}

vec2 EvalPoly(vec2 poly[size], int degree, vec2 x)
{
	vec2 sum = poly[0];
	for (int i = 1; i < degree+1; i++)
	{
		sum = mat2(sum,-sum.y,sum.x) * x + poly[i];
	}
	return sum;
}

void FindAllRootsDurand(inout vec2 roots[size-1], vec2 poly[size], int degree)
{
	vec2 leading = poly[0];
	for (int i = 0; i < size; i++)
	{
		poly[i] = cDiv(poly[i], leading);
	}

	vec2 oldRoots[size-1];
	for (int i = 0; i < size - 1; i++)
	{
		roots[i] = complexPow(vec2(0.4,0.9), i);
		oldRoots[i] = vec2(0);
	}
    
	for (int iteration = 0; iteration < maxIterations; iteration++)
	{
		for (int i = 0; i < size - 1; i++)
		{
			oldRoots[i] = roots[i];
		}
		vec2 delta = vec2(0);

		for (int i = 0; i < size - 1; i++)
		{
			vec2 product = vec2(1,0);
			for (int j = 0; j < size - 1; j++)
			{
				if (i==j)
				{
					continue;
				}

				product = cMul(product, roots[i]-oldRoots[j]);
			}
			vec2 y = EvalPoly(poly, degree, roots[i]);
			vec2 d = cDiv(y, product);
			delta+=abs(d);
			roots[i] -= d;
		}

		delta/=size;
		if (dot(delta, delta) < epsilon*epsilon)
		{
			break;
		}
	}
}

</polynomial>

<intersection>
bool InsideBox(vec2 v, vec4 box)
{
	vec2 s = step(box.xy, v) - step(box.zw, v);
	return bool(s.x * s.y);
}
</intersection>