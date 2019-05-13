#version 330 core


layout(location = 0) out vec4 color;
in vec4 pos;

uniform int maxIterations = 1024;
uniform float iterationLog = log(float(4096));
uniform float offsetX = 0;
uniform float offsetY = 0;
uniform float zoom = 1;
uniform float power = 2;

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

vec2 complexTan(vec2 v)
{
	//vec2 d = v * 1024;
	//vec2 d = v * 4;
	vec2 d = v * 2;

	float denominator = cos(d.x)+cosh(d.y);

	return vec2(sin(d.x)/denominator, sinh(d.y)/denominator);
}

void main()
{
	vec2 npos = vec2((offsetX) + pos.x * zoom, (offsetY) + pos.y * zoom);
#if 1
	vec2 z = vec2(0,0);

	float modulo;
	float angle;

	// Polar iteration
	vec2 trap = vec2(abs(npos));

	int i = maxIterations;
	for (; i > 0; i--)
	{
		modulo = length(z);


		angle = atan(z.y, z.x) * power;
		modulo = pow(modulo, power);
		if (modulo > 2)
		{
			break;
		}

		z = npos + modulo * vec2(cos(angle), sin(angle));
		//z = complexTan(z);
		
		//trap.x = (max(z.x, trap.x) + sinh(trap.x))*0.5; // change break position
		//trap.y = (max(z.y, trap.y) - tanh(trap.y))*0.5;

		//trap = max(z, abs(z));
		/*if (z.x < npos.x + zoom * 0.1 && z.x > npos.x - zoom * 0.1)
		{
			trap = z;
		}*/

		
		float epsilon = pow(0.001,zoom);
		if (z.x < npos.x + epsilon && z.x > npos.x - epsilon)
		{
			//trap = mix(z,trap, 0.99);

			trap = z;
		}
		
		
		/*float epsilon = pow(0.001,zoom);
		if (z.x < npos.x + epsilon && z.x > npos.x - epsilon && z.y < npos.y + epsilon && z.y > npos.y - epsilon)
		{
			trap = z;
		}*/
		
		/*float epsilon = pow(0.1,zoom);
		if (z.x<(tan(npos+epsilon)).x)
		{
			trap = z;
		}*/
	}
#else
	float x = 0;
	float y = 0;

	float y2;
	float x2;

	float i = maxIterations;
	for (; i > 0; i--)
	{
		y2 = y * y;
		x2 = x * x;

		if (x2 + y2 > 4)
		{
			break;
		}

		y = (x + x) * y + npos.y;
		x = x2 - y2 + npos.x;
	}
#endif
	color = vec4(trap.xy, length(trap)*0.5, 1);

	//color = mix(vec4(trap.xy, length(trap)*0.5, 1), GetColor(float(maxIterations - i)), 0.5);
	//color = vec4(distance(trap, vec2(real, imag)), 0, 0, 1);
};