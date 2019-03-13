#version 330 core

layout(location = 0) out vec4 color;
in vec4 pos;

const int width = 1080;
const int height = 1080;

uniform float time;

uniform vec3 eye;

vec4 orb;

// For pow 2
#define different 0

float DE(vec3 p)
{
	const int Power = 8;
	const int maxIterations = 10;
	const float bailout = 1.15;

	vec3 z = p;
	float dr = 1.0;
	float r = 0.0;

	for (int i = 0; i < maxIterations; i++)
	{
		r = length(z);

		if (r > bailout) break;
		
		// convert to polar coordinates
#if different > 0
		float theta = asin( z.z/r );
		float phi = atan( z.y,z.x );
#else
		float theta = acos(z.z/r);
		float phi = atan(z.y,z.x);
#endif
		dr =  pow(r, Power - 1.0) * Power * dr + 1.0;
		
		// scale and rotate the point
		float zr = pow(r, Power);
		theta = theta * Power;
		phi = phi * Power;
		
		// convert back to cartesian coordinates
#if different > 0
		z = zr*vec3( cos(theta)*cos(phi), cos(theta)*sin(phi), sin(theta));
#else
		z = zr * vec3(sin(theta) * cos(phi), sin(phi) * sin(theta), cos(theta));
#endif
		z+=p;
	}

	return 0.5 * log(r) * r / dr;
}

float trace(vec3 origin, vec3 ray)
{
	float t = 0.;

	for (int i = 0; i < 100; i++)
	{
		vec3 p = origin + ray * t;

		float distance = DE(p);//, (1.1 + 0.5*smoothstep( -0.3, 0.3, cos(0.1*time) )));
		t += distance * 0.6;
	}

	return t;
}

void main()
{
	vec2 uv = (gl_FragCoord.xy / vec2(width, height));
	uv = uv * 2.0 - 1.0;

	uv.x *= float(width) / height;

	vec3 r = normalize(vec3(uv, 1.0));
	
	//float the = time;
	//r.xz *= mat2(cos(the), -sin(the), sin(the), cos(the));

	vec3 origin = vec3(eye.y * -1, 0, eye.x);

	float t = trace(origin, r);

	float tmod= mod(t, 1);

    color = vec4(tmod, tmod, tmod, 1.0);
}
