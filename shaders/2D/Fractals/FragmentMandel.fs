#version 330 core


layout(location = 0) out vec4 color;

uniform vec2 screenSize = vec2(1920, 1080);
uniform int maxIterations = 4096;
uniform float iterationLog = log(float(4096));
uniform float zoom = 1;
uniform vec2 position;

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
    vec2 c = (2*gl_FragCoord.xy-screenSize)/screenSize.y * zoom + position;

	vec2 z = c;

	int i = 0;
	for (; i < maxIterations; i++)
	{
		
		if (dot(z, z) > 4.) break;
		z = mat2(z, -z.y, z.x) * z + c;
	}

	color = GetColor(float(i));
};