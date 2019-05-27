%uniforms%
uniform float power = 1;
uniform float genericParameter = 1;
uniform vec3 sun;
%/uniforms%

%constants%
const float antiAliasing = 2;
%/constants%

%DE%
void sphereFold(inout vec3 z, inout float dz)
{
	float minRadius2 = power/2;
	float fixedRadius2 = power * 2;
	float r2 = dot(z,z);
	if (r2<minRadius2) { 
		// linear inner scaling
		float temp = (fixedRadius2/minRadius2);
		z *= temp;
		dz*= temp;
	} else if (r2<fixedRadius2) { 
		// this is the actual sphere inversion
		float temp =(fixedRadius2/r2);
		z *= temp;
		dz*= temp;
	}
}

void boxFold(inout vec3 z, inout float dz) {
	float foldingLimit =power/2;
	z = clamp(z, -foldingLimit, foldingLimit) * 2.0 - z;
}
float DistanceEstimator(vec3 z, out vec4 resColor, float _)
{
	float Scale = genericParameter;
	vec3 c = z;
	float dr = 1.0;
	float m;
	vec4 trap = vec4(0,0,0,0);
	for (int n = 0; n < 8; n++) 
	{
		boxFold(z,dr);     
		sphereFold(z,dr);
		
		//boxFold(z,dr);     
		//sphereFold(z,dr);  
 		
        z=Scale*z + c;  // Scale & Translate
		
		//z.x *= cos(z.x);
		
		//z.z = cos(z.z)*sin(z.z);
		//z.y = cos(z.y)*sin(z.y);

        dr = dr*abs(Scale)+1.0;

		trap = max(trap, vec4(abs(z), m));

		m = dot(z,z);
	}
	resColor = trap;
	float r = length(z);
	return r/abs(dr);
}
%/DE%

%Color%
col = vec3(0.01);
col = mix(col, vec3(0.54,0.3,0.07), clamp(trap.y,0.0,1.0)); // Inner
col = mix(col, vec3(0.02,0.4,0.30), clamp(trap.z*trap.z,0.0,1.0));
col = mix(col, vec3(0.78, 0.5, 0.13), clamp(pow(trap.w,6.0),0.0,1.0)); // Stripes
col *= 0.5;
%/Color%

%main%
	vec2 uv = gl_FragCoord.xy / screenSize;
	uv = uv * 2.0 - 1.0;

	uv.x *= float(screenSize.x) / float(screenSize.y);

	vec3 direction = normalize(vec3(uv.xy, 1));

	direction.zy *= pitchMatrix;

	direction.xz *= yawMatrix;
	direction.xy *= rollMatrix;
	direction.y *= worldFlip;
	
	
	vec3 col = render(Ray(vec3(eye.z, eye.y * worldFlip, eye.x), direction.xyz));

    color = vec4(col.xyz, 1.0);
%/main%

%mainAA%

	vec3 col = vec3(0.0);
	for (int i = 0; i < antiAliasing; i++)
	{
		for (int j = 0; j < antiAliasing; j++)
		{
			vec2 frag = gl_FragCoord.xy;
			frag.x += float(i)/antiAliasing;
			frag.y += float(j)/antiAliasing;
			vec2 uv = frag / screenSize;
			uv = uv * 2.0 - 1.0;
			uv.x *= float(screenSize.x) / float(screenSize.y);

			vec3 direction = normalize(vec3(uv.xy, 1));

			direction.zy *= pitchMatrix;

			direction.xz *= yawMatrix;
			direction.xy *= rollMatrix;
			direction.y *= worldFlip;
	

			Ray	ray = Ray(vec3(eye.z, eye.y * worldFlip, eye.x), direction.xyz);
			col += render(ray);
	
			//vec3 col = render(Ray(vec3(eye.z, eye.y * worldFlip, eye.x), direction.xyz));
		}
	}
	col /= float(antiAliasing*antiAliasing);

    color = vec4(col.xyz, 1.0);
%/mainAA%

struct Ray
{
	vec3 origin;
	vec3 dir;
};

%DE%
// Distance estimator

float Map(vec3 start, out vec4 resColor)
{
#if 1
	vec4 trap;
	float t = DistanceEstimator(start, trap, power);
	resColor = trap;
	return t;
#else

	vec4 trap1;
	vec4 trap2;

	float dist1 = DistanceEstimator(start, trap1, power);
	float dist2 = DistanceEstimator(start, trap2, power / log(power));

	if (dist1 < dist2)
	{
		resColor = trap1;
		return dist1;
	}
	else
	{
		resColor = trap2;
		return dist2;
	}
#endif
}

vec2 isphere(vec4 sph, vec3 origin, vec3 ray)
{
    vec3 oc = origin - sph.xyz;
    
	float b = dot(oc,ray);
	float c = dot(oc,oc) - sph.w*sph.w;
    float h = b*b - c;
    
    if( h<0.0 ) return vec2(-1.0);

    h = sqrt( h );

    return -b + vec2(-h,h);
}

#if fractal == 0
float trace(Ray ray, out vec4 trapOut, float px, out float percentSteps)
{
    float res = -1.0;

    //bounding sphere
    vec2 dis = isphere( vec4(0.0,0.0,0.0,1.25), ray.origin, ray.dir);
    if(dis.y < 0.0)
        return -1.0;

    dis.x = max(dis.x, 0.0);
    dis.y = min(dis.y, 10.0);

    // raymarch fractal distance field
	vec4 trap;

	float t = dis.x;
	int i = 0;
	for(; i<maxSteps; i++  )
    { 
        vec3 pos = ray.origin + ray.dir * t;
		float h = Map(pos, trap);
        float th = 0.25 * px * t;

		if(t>dis.y || h < th)
		{
			break;
        }
		t += h;
    }

	percentSteps = float(i) / maxSteps;
    percentSteps *= (percentSteps * 4); // Smoothing out, making the circle thing disappear
	percentSteps = 1;
    if(t < dis.y)
    {
        trapOut = trap;
        res = t;
    }

    return res;
}
#endif
#if fractal == 1
float trace(Ray ray, out vec4 trapOut, float px, out float percentSteps)
{
    float res = -1.0;

	vec4 trap;

	float t = 0;
	int i = 0;
	for(; i<maxSteps; i++)
    { 
        vec3 pos = ray.origin + ray.dir * t;
		float h = Map(pos, trap);
        float th = 0.25 * px * t;

		if(h<th || h>power*4)
		{
			break;
        }
		t += h;
    }

	percentSteps = 0;

	if (t < power*8)
	{
	trapOut = trap;
        res = t;
	}
	else
	{
		res = -1;
	}

    return res;
}