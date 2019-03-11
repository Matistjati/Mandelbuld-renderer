#version 330 core


layout(location = 0) out vec4 color;
in vec4 pos;

uniform int maxIterations = 1024;
uniform float offsetX = 0;
uniform float offsetY = 0;
uniform float zoom = 1;
const float log4 = log(float(4));

vec4 GetColor(float iterations)
{
	float quarterIt = iterations / 4; 

	vec4 localColor = vec4(0.0, 0.0, 0.0, 1.0);

	if (iterations + 2 > maxIterations)
	{
		return vec4(0, 0, 0, 1.0);
	}
	else
	{
		float c = 3 * log(iterations) / log4;

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

	color = GetColor((x2 + y2) / 4 + i / maxIterations); // Do miss with the denominator
};