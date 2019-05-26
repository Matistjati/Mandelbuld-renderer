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
float DistanceEstimator(vec3 z, out vec4 resColor, float ignore)
{
	float Scale = genericParameter;
	vec3 offset = z;
	float dr = 1.0;
	float m;
	vec4 trap = vec4(0,0,0,0);
	for (int n = 0; n < 8; n++) 
	{
		boxFold(z,dr);       // Reflect
		sphereFold(z,dr);    // Sphere Inversion
 		
        z=Scale*z + offset;  // Scale & Translate
        dr = dr*abs(Scale)+1.0;

		trap = max(trap, distance(z, vec3(0.2, 0.475, 0.236)));

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

		if(h<th || h>power*8)
		{
			break;
        }
		t += h;
    }

	percentSteps = 0;

	if (t < power*4)
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
#endif
vec3 calculateNormal(vec3 p)
{
    const vec3 small_step = vec3(0.001, 0.0, 0.0);

	vec3 gradient;
	vec4 temp;

    gradient.x = Map(p + small_step.xyy, temp) - Map(p - small_step.xyy, temp);
    gradient.y = Map(p + small_step.yxy, temp) - Map(p - small_step.yxy, temp);
    gradient.z = Map(p + small_step.yyx, temp) - Map(p - small_step.yyx, temp);

    return normalize(gradient);
}

float SoftShadow(Ray ray, float k)
{
    float result = 1.0;
    float t = 0.0;
	
	vec4 temp;

    for(int i = 0; i < 64; i++)
    {
        float h = Map(ray.origin + ray.dir * t, temp);
        result = min(result, k * h / t);

        if(result < 0.001) break;

        t += clamp(h, 0.01, 32);
    }
    return clamp(result, 0.0, 1.0);
}

vec3 render(Ray ray)
{
	const float zoom = .01;
	float px = 2.0 / (screenSize.y * zoom);
	vec4 trap;
	float steps;

	float t = trace(ray, trap, zoom, steps);

	vec3 col;

	// Color the sky if we don't hit the fractal
	if(t < 0.0)
    {
		// Sky gradient
		// Blue sky
     	col = vec3(0.8, 0.95, 1.0) * (0.6 + 0.4 * ray.dir.y);
		// Cave
     	//col = vec3(0.2, 0.05, 0.0) * (0.4 + 0.6 * ray.dir.y);

		// Sun
		col += sunBrightness * vec3(0.8,0.7,0.5) * pow(clamp(dot(ray.dir, sun), 0.0, 1.0), sunTightness);

		col += vec3(0.556, 0.843, 0.415) * steps;
	}
	else
	{
		%Color%

		// Lighting

		// The end position of the ray
		vec3 pos = (ray.origin + ray.dir * t);
		vec3 normal = calculateNormal(pos);
		Ray fractalToSun = Ray(pos + 0.001 * normal, sun);
		vec3 fractalToSunDir = normalize(sun - ray.dir);
		float occlusion = clamp(0.05*log(trap.x),0.0,1.0);
		float fakeSSS = clamp(1.0+dot(ray.dir,normal),0.0,1.0);

		// Sun
		float shadow = SoftShadow(fractalToSun, 32.);
		float diffuse = clamp(dot(sun, normal), 0.0, 1.0 ) * shadow;
		float specular = pow(clamp(dot(normal,fractalToSunDir),0.0,1.0), 32.0 )*diffuse*(0.04+0.96*pow(clamp(1.0-dot(fractalToSunDir,sun),0.0,1.0),5.0));

		// Bounce
		float diffuse2 = clamp( 0.5 + 0.5*dot(light, normal), 0.0, 1.0 )*occlusion;

		// Sky
		float diffuse3 = (0.7+0.3*normal.y)*(0.2+0.8*occlusion);

		vec3 light = vec3(0.0); 
		light += 7.0*vec3(1.50,1.10,0.70)*diffuse;
		light += 4.0*vec3(0.25,0.20,0.15)*diffuse2;
		light += 1.5*vec3(0.10,0.20,0.30)*diffuse3;
		light += 2.5*vec3(0.35,0.30,0.25)*(0.05+0.95*occlusion); // ambient
		light += 4*fakeSSS*occlusion;                          // fake SSS


		col *= light;
		col = pow( col, vec3(0.7,0.9,1.0) );                  // fake SSS
		col += specular * 15.;

		vec3 reflection = reflect( ray.dir, normal );

		//col += 8.0*vec3(0.8,0.9,1.0)*(0.2+0.8*occlusion)*(0.03+0.97*pow(fakeSSS,5.0))*smoothstep(0.0,0.1,reflection.y )*SoftShadow( Ray(pos+0.01*normal, reflection), 2.0 );
		//col = vec3(occlusion*occlusion);
	}

	return col;
}

void main()
{
	%main%
	%mainAA%
}