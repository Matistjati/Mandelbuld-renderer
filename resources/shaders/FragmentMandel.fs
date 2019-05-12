#version 330 core


layout(location = 0) out vec4 color;
in vec4 pos;

uniform int maxIterations = 1024;
uniform float iterationLog = log(float(4096));
uniform float offsetX = 0;
uniform float offsetY = 0;
uniform float zoom = 1;
uniform float power = 2;
uniform float escapeRadius = 4;

vec4 GetColor(float iterations)
{
	vec4 localColor = vec4(0.0, 0.0, 0.0, 1.0);

	if (iterations + 2 > maxIterations)
	{
		return vec4(0, 0, 0, 1.0);
	}
	else
	{
		float c = 3 * log(iterations) / iterationLog;

		if (c < 1)
		{
			return vec4(c, 0.0, 0.0, 1.0);
		}
		else if (c < 2)
		{
			return vec4(1, c - 1, 0.0, 1.0);
		}
		else
		{
			return vec4(1, 1, c - 2, 1.0);
		}
	}
}

void main()
{
	vec2 npos = vec2((offsetX) + pos.x * zoom, (offsetY) + pos.y * zoom);

	float real = 0;
	float imag = 0;

	float modulo;
	float angle;

	// Polar iteration
	vec2 trap = vec2(0,0);

	int i = maxIterations;
	for (; i > 0; i--)
	{
		modulo = length(vec2(real, imag));
		angle = atan(imag, real);

		modulo = pow(modulo, power);
		angle = angle * power;

		if (modulo > escapeRadius)
		{
			break;
		}

		real = cos(angle) * modulo;
		imag = sin(angle) * modulo;

		real += npos.x;
		imag += npos.y;
		trap.x = (max(real, trap.x) + sinh(trap.x))*0.5;
		trap.y = (max(imag, trap.y) - tanh(trap.y))*0.5;
	}

	color = vec4(trap.xy, length(trap)*0.5, 1);//GetColor(float(maxIterations - i));
};