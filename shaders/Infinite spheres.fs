#version 330 core

layout(location = 0) out vec4 color;
in vec4 pos;

const int width = 1080;
const int height = 1080;

uniform float time;

uniform vec3 eye;

float DE(vec3 p)
{
	vec3 q = fract(p) * 2.0 - 1.0;

	return length(q) - 0.25;(abs(sin(time / 2)));
}

float trace(vec3 origin, vec3 ray)
{
	float t = 0.;

	for (int i = 0; i < 32; i++)
	{
		float distance = DE(origin + ray * t);
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

	float fog = 1.0 / (1. + t * t * 0.1);


	vec3 fc = vec3(fog);

    color = vec4(fc, 1.0);
}
