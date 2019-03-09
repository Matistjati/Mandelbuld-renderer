#version 330 core

const int maxIterations = 1024;
const float iterationLog = log(float(maxIterations));

layout(location = 0) out vec4 color;
in vec4 pos;

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
	float x = 0;
	float y = 0;

	float y2;
	float x2;

	int i = maxIterations;
	for (; i > 0; i--)
	{
		y2 = y * y;
		x2 = x * x;

		if (x2 + y2 > 4)
		{
			break;
		}

		y = 2 * x * y + pos.y;
		x = x2 - y2 + pos.x;


	}

	color = GetColor(float(maxIterations - i));
};