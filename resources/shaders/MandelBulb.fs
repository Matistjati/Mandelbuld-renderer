#version 330 core

layout(location = 0) out vec4 color;

uniform int width = 1080;
uniform int height = 1080;
uniform float power = 8;
uniform int worldFlip = -1;

uniform mat2 yawMatrix;
uniform mat2 pitchMatrix;
uniform float time;

uniform vec3 eye;

const float epsilon = 0.001;
const int maxIterations = 4;
const int maxSteps = 100;
const float bailout = 1.15;
const float stepMultiplier = 0.6;
const int outerSphereRadius = 3;

struct Ray
{
	vec3 origin;
	vec3 dir;
};

float DE(vec3 p, out vec4 resColor)
{
	vec3 w = p;
    float m = dot(w,w);

    vec4 trap = vec4(abs(w),m);
	float dz = 1.0;
    
    
	for(int i = 0; i < maxIterations; i++)
    {
#if 0
        float m2 = m*m;
        float m4 = m2*m2;
		dz = power*sqrt(m4*m2*m)*dz + 1.0;

        float x = w.x; float x2 = x*x; float x4 = x2*x2;
        float y = w.y; float y2 = y*y; float y4 = y2*y2;
        float z = w.z; float z2 = z*z; float z4 = z2*z2;

        float k3 = x2 + z2;
        float k2 = inversesqrt( pow(k3, power - 1) );
        float k1 = x4 + y4 + z4 - 6.0*y2*z2 - 6.0*x2*y2 + 2.0*z2*x2;
        float k4 = x2 - y2 + z2;

        w.x = p.x +  64.0*x*y*z*(x2-z2)*k4*(x4-6.0*x2*z2+z4)*k1*k2;
        w.y = p.y + -16.0*y2*k3*k4*k4 + k1*k1;
        w.z = p.z +  -power*y*k4*(x4*x4 - 28.0*x4*x2*z2 + 70.0*x4*z4 - 28.0*x2*z2*z4 + z4*z4)*k1*k2;
#else
        dz = power*pow(sqrt(m),power-1)*dz + 1.0;
		//dz = 8.0*pow(m,3.5)*dz + 1.0;
        
        float r = length(w);
        float b = power*acos( w.y/r);
        float a = power*atan( w.x, w.z );
        w = p + pow(r,power) * vec3( sin(b)*sin(a), cos(b), sin(b)*cos(a) );
#endif
        
        trap = min( trap, vec4(abs(w),m) );

        m = dot(w,w);
		if( m > 256.0 )
            break;
	}
	
	resColor = vec4(m,trap.yzw);

    return 0.25* log(m)*sqrt(m)/dz;
}

vec2 isphere(vec4 sph, vec3 origin, vec3 ray)
{
	// TODO How the fuck does this even work?? cpu render for intuition
    vec3 oc = origin - sph.xyz;
    
	float b = dot(oc,ray);
	float c = dot(oc,oc) - sph.w*sph.w;
    float h = b*b - c;
    
    if( h<0.0 ) return vec2(-1.0);

    h = sqrt( h );

    return -b + vec2(-h,h);
}

float trace(Ray ray, out vec4 rescol, in float px, out float gradient)
{
    float res = -1.0;

    // bounding sphere
    vec2 dis = isphere( vec4(0.0,0.0,0.0,1.25), ray.origin, ray.dir);
    if( dis.y<0.0 )
        return -1.0;

    dis.x = max(dis.x, 0.0);
    dis.y = min(dis.y, 10.0);

    // raymarch fractal distance field
	vec4 trap;

	float t = dis.x;
	int i=0;
	for(; i<maxSteps; i++  )
    { 
        vec3 pos = ray.origin + ray.dir * t;
		float h = DE(pos, trap);
        float th = 0.25*px*t;
		if( t>dis.y || h<th ) break;
        t += h;
    }
    
	gradient = 1 - float(i) / float(maxSteps);
    
    if(t < dis.y)
    {
        rescol = trap;
        res = t;
    }

    return res;
}

vec3 calculateNormal(vec3 p)
{
    const vec3 small_step = vec3(0.001, 0.0, 0.0);

	vec3 gradient;
	vec4 temp;

    gradient.x = DE(p + small_step.xyy, temp) - DE(p - small_step.xyy, temp);
    gradient.y = DE(p + small_step.yxy, temp) - DE(p - small_step.yxy, temp);
    gradient.z = DE(p + small_step.yyx, temp) - DE(p - small_step.yyx, temp);

    return normalize(gradient);
}

const vec3 sun = vec3(0.577, 0.577, -0.577);

vec3 render(Ray ray)
{
	const float zoom = .01;
	float px = 2.0 / (height * zoom);
	vec4 trap;

	float iterations;
	float t = trace(ray, trap, zoom, iterations);

	vec3 col;

	// Color the sky if we don't hit the fractal
	if(t < 0.0)
    {
		// Sky gradient
     	col = vec3(0.8, 0.95, 1.0) * (0.6 + 0.4 * ray.dir.y);

		// Sun

		// TODO How the fuck does this even work?? cpu render for intuition
		const float sunSize = 5.0;
		col += sunSize * vec3(0.8,0.7,0.5) * pow(clamp(dot(ray.dir, sun), 0.0, 1.0), 32.0);
	}
	else
	{
		col = vec3(iterations, iterations, iterations);
	}

	return sqrt(col);
}

void main()
{
	vec2 uv = (gl_FragCoord.xy / vec2(width, height));
	uv = uv * 2.0 - 1.0;

	uv.x *= float(width) / height;

	vec3 direction = normalize(vec3(uv.xy, 1));

	direction.zy *= pitchMatrix;

	direction.xz *= yawMatrix;
	direction.y *= worldFlip;
	
	vec3 col = render(Ray(vec3(eye.z, eye.y * worldFlip, eye.x), direction.xyz));

    color = vec4(col.xyz, 1.0);
}