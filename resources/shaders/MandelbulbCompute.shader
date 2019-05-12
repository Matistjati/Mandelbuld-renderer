#version 430 core

uniform int width = 1080;
uniform int height = 1080;
uniform float power = 8;
uniform int worldFlip = -1;

uniform mat2 yawMatrix;
uniform mat2 pitchMatrix;
uniform mat2 rollMatrix;
uniform vec3 sun;

uniform vec3 eye;
const float epsilon = 0.001;
const int maxIterations = 8192;
const int maxSteps = 100;
const float bailout = 1.15;
const float sunBrightness = 1.0;
const float sunTightness = 16.0;
const vec3 light = vec3(-0.707, 0.000, 0.707);

struct Pixel
{
	float r;
	float g;
	float b;
	float a;
};

struct Ray
{
	vec3 origin;
	vec3 dir;
};

layout(std430, binding = 4) buffer Pixels
{
	Pixel p[];
};

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

float DistanceEstimator(vec3 start, out vec4 resColor, float Power)
{
	vec3 w = start;
	float m = dot(w, w);

	vec4 trap = vec4(abs(w), m);
	float dz = 1.0;


	for (int i = 0; i < maxIterations; i++)
	{
#if 0
		float m2 = m * m;
		float m4 = m2 * m2;
		dz = Power * sqrt(m4*m2*m)*dz + 1.0;

		float x = w.x; float x2 = x * x; float x4 = x2 * x2;
		float y = w.y; float y2 = y * y; float y4 = y2 * y2;
		float z = w.z; float z2 = z * z; float z4 = z2 * z2;

		float k3 = x2 + z2;
		float k2 = inversesqrt(pow(k3, Power - 1));
		float k1 = x4 + y4 + z4 - 6.0*y2*z2 - 6.0*x2*y2 + 2.0*z2*x2;
		float k4 = x2 - y2 + z2;

		w.x = start.x + 64.0*x*y*z*(x2 - z2)*k4*(x4 - 6.0*x2*z2 + z4)*k1*k2;
		w.y = start.y + -16.0*y2*k3*k4*k4 + k1 * k1;
		w.z = start.z + -Power * y*k4*(x4*x4 - 28.0*x4*x2*z2 + 70.0*x4*z4 - 28.0*x2*z2*z4 + z4 * z4)*k1*k2;
#else
		dz = (Power * pow(sqrt(m), Power - 1)) * dz + 1.0;
		//dz = Power*pow(m,(Power-1)*0.5)*dz + 1.0;

		float r = length(w);
		float theta = Power * atan(w.x, w.z);
		float phi = Power * acos(w.y / r);

		// Fun alternative: reverse sin and cos
		w = start + pow(r, Power) * vec3(sin(phi) * sin(theta), cos(phi), sin(phi) * cos(theta));
#endif

		trap = min(trap, vec4(abs(w), m));

		m = dot(w, w);
		if (m > 256.0)
			break;
	}

	resColor = vec4(m, trap.yzw);

	return 0.4* log(m)*sqrt(m) / dz;
}

float Map(vec3 start, out vec4 resColor)
{
#if 1
	return DistanceEstimator(start, resColor, power);
#else

	vec4 trap1;
	vec4 trap2;

	float dist1 = DistanceEstimator(start, trap1, power);
	float dist2 = DistanceEstimator(start, trap2, power / log(power));

	if (dist1 < dist2)
	{
		resColor = trap1;
		return min(dist1, dist2);
	}
	else
	{
		resColor = trap2;
		return min(dist1, dist2);
	}
#endif
}

vec2 isphere(vec4 sph, vec3 origin, vec3 ray)
{
	vec3 oc = origin - sph.xyz;

	float b = dot(oc, ray);
	float c = dot(oc, oc) - sph.w*sph.w;
	float h = b * b - c;

	if (h < 0.0) return vec2(-1.0);

	h = sqrt(h);

	return -b + vec2(-h, h);
}

float trace(Ray ray, out vec4 trapOut, float px, out float percentSteps)
{
	float res = -1.0;

	// bounding sphere
	vec2 dis = isphere(vec4(0.0, 0.0, 0.0, 1.25), ray.origin, ray.dir);
	if (dis.y < 0.0)
		return -1.0;

	dis.x = max(dis.x, 0.0);
	dis.y = min(dis.y, 10.0);

	// raymarch fractal distance field
	vec4 trap;

	float t = dis.x;
	int i = 0;
	for (; i < maxSteps; i++)
	{
		vec3 pos = ray.origin + ray.dir * t;
		float h = Map(pos, trap);
		float th = 0.25 * px * t;

		if (t > dis.y || h < th)
		{
			break;
		}
		t += h;
	}

	percentSteps = float(i) / maxSteps;
	percentSteps *= (percentSteps * 4); // Smoothing out, making the circle thing disappear

#if 0
	if (t < dis.y)
	{
		trapOut = trap;
		res = t;
	}
#else // Snowglobe sort of thing
	trapOut = trap;
	res = t;
#endif

	return res;
}

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

	for (int i = 0; i < 64; i++)
	{
		float h = Map(ray.origin + ray.dir * t, temp);
		result = min(result, k * h / t);

		if (result < epsilon) break;

		t += clamp(h, 0.01, 32.);
	}
	return clamp(result, 0.0, 1.0);
}

vec3 render(Ray ray)
{
	const float zoom = .01;
	float px = 2.0 / (height * zoom);
	vec4 trap;
	float steps;

	float t = trace(ray, trap, zoom, steps);

	vec3 col;

	// Color the sky if we don't hit the fractal
	if (t < 0.0)
	{
		// Sky gradient
		// Blue sky
		col = vec3(0.8, 0.95, 1.0) * (0.6 + 0.4 * ray.dir.y);


		// Sun
		col += sunBrightness * vec3(0.8, 0.7, 0.5) * pow(clamp(dot(ray.dir, sun), 0.0, 1.0), sunTightness);

		col += vec3(0.556, 0.843, 0.415) * steps;
	}
	else
	{
		col = vec3(0.01);
		col = mix(col, vec3(0.54, 0.3, 0.07), clamp(trap.y, 0.0, 1.0)); // Inner
		col = mix(col, vec3(0.02, 0.4, 0.30), clamp(trap.z*trap.z, 0.0, 1.0));
		col = mix(col, vec3(0.15, 0.4, 0.04), clamp(pow(trap.w, 6.0), 0.0, 1.0)); // Stripes
		col *= 0.5;

		// The end position of the ray
		vec3 pos = (ray.origin + ray.dir * t);
		vec3 normal = calculateNormal(pos);
		Ray fractalToSun = Ray(pos + 0.001 * normal, sun);
		vec3 fractalToSunDir = normalize(sun - ray.dir);
		float occlusion = clamp(0.05*log(trap.x), 0.0, 1.0);
		float fakeSSS = clamp(1.0 + dot(ray.dir, normal), 0.0, 1.0);

		// Sun
		float shadow = SoftShadow(fractalToSun, 32.);
		float diffuse = clamp(dot(sun, normal), 0.0, 1.0) * shadow;
		float specular = pow(clamp(dot(normal, fractalToSunDir), 0.0, 1.0), 32.0)*diffuse*(0.04 + 0.96*pow(clamp(1.0 - dot(fractalToSunDir, sun), 0.0, 1.0), 5.0));

		// Bounce
		float diffuse2 = clamp(0.5 + 0.5*dot(light, normal), 0.0, 1.0)*occlusion;

		// Sky
		float diffuse3 = (0.7 + 0.3*normal.y)*(0.2 + 0.8*occlusion);

		vec3 light = vec3(0.0);
		light += 7.0*vec3(1.50, 1.10, 0.70)*diffuse;
		light += 4.0*vec3(0.25, 0.20, 0.15)*diffuse2;
		light += 1.5*vec3(0.10, 0.20, 0.30)*diffuse3;
		light += 2.5*vec3(0.35, 0.30, 0.25)*(0.05 + 0.95*occlusion); // ambient
		light += 4 * fakeSSS*occlusion;                          // fake SSS


		col *= light;
		col = pow(col, vec3(0.7, 0.9, 1.0));                  // fake SSS
		col += specular * 15.;

		vec3 reflection = reflect(ray.dir, normal);

		//col += 8.0*vec3(0.8,0.9,1.0)*(0.2+0.8*occlusion)*(0.03+0.97*pow(fakeSSS,5.0))*smoothstep(0.0,0.1,reflection.y )*SoftShadow( Ray(pos+0.01*normal, reflection), 2.0 );
		//col = vec3(occlusion*occlusion);
	}

	return col;
}

void main()
{
	vec2 fragCoord = vec2(float(gl_GlobalInvocationID.x), float(gl_GlobalInvocationID.y));
	//fragCoord.xy -= 2;
	vec2 uv = (fragCoord.xy / vec2(float(width), float(height)));
	uv = uv * 2.0 - 1.0;

	uv.x *= float(width) / float(height);

	vec3 direction = normalize(vec3(uv.xy, 1));

	direction.zy *= pitchMatrix;

	direction.xz *= yawMatrix;
	direction.xy *= rollMatrix;
	direction.y *= worldFlip;

	vec3 newEye = vec3(eye.z, eye.y * worldFlip, eye.x);
	vec3 col = render(Ray(newEye, direction));

	p[gl_GlobalInvocationID.x*width + gl_GlobalInvocationID.y].r = col.r;
	p[gl_GlobalInvocationID.x*width + gl_GlobalInvocationID.y].g = col.g;
	p[gl_GlobalInvocationID.x*width + gl_GlobalInvocationID.y].b = col.b;
	p[gl_GlobalInvocationID.x*width + gl_GlobalInvocationID.y].a = 1;

	//p[gl_GlobalInvocationID.x*width + gl_GlobalInvocationID.y].r = gl_GlobalInvocationID.y*gl_GlobalInvocationID.x;

}