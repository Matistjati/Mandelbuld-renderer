#version 330 core


layout(location = 0) out vec4 color;
in vec4 pos;

uniform int maxIterations = 1024;
uniform float iterationLog = log(float(4096));
uniform float offsetX = 0;
uniform float offsetY = 0;
uniform float zoom = 1;
uniform float power = 2;
uniform vec2 cursor1 = vec2(0.3, -0.3);
uniform vec2 cursor2 = vec2(0.47282399071, -0.91782235329);
uniform vec2 cursor3 = vec2(0.39699134473, 0.80443820146);
uniform float param = 30;
uniform float t;

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

float firstDerivative(float x, vec2 p, float k)
{
	return 2*(-p.x-2*(p.y+2)*k*x+2*k*k*x*x*x+x);
}

float secondDerivative(float x, vec2 p, float k)
{
	return -4*k*(p.y+2)+12*k*k*x*x+2;
}


float distFromCurve(vec2 p, float k)
{
	// Incorrect but cool
	/*float x = 1;
	for	(int i = 0; i < 8; i++)
	{
		float xdelta = x - p.x;
		xdelta *= xdelta;

		float ydelta = ((k * x * x) - 2) - p.y;
		// 	kx^2 - 2
		ydelta *= ydelta;


		float top = 2*x-p.x + (4*k*x*(k*x*x-2-p.y));

		float a = x - p.x;
		a *= a;
		float b = k*x*x-2-p.y;
		b *= b;

		x -= sqrt(xdelta + ydelta)/(top/(2*sqrt(a+b)));
	}
	return x;
	*/
	
	float x = 0;
	for	(int i = 0; i < 5; i++)
	{
		x -= firstDerivative(x, p, k)/secondDerivative(x, p, k);
	}

	// Did we hit the right extreme point?
	if (secondDerivative(x, p, k) > 0)
	{
		return x;
	}
	else
	{
		x = -1;
		for	(int i = 0; i < 5; i++)
		{
			x -= firstDerivative(x, p, k)/secondDerivative(x, p, k);
		}
		return x;
	}
}

void main()
{
	vec2 npos = vec2((offsetX) + pos.x * zoom, (offsetY) + pos.y * zoom);
#if 1
	vec2 z = vec2(0,0);

	float modulo;
	float angle;

	// Polar iteration
	vec3 trap = vec3(10000, 10000, 10000);
	vec3 trap2 = vec3(10000, 10000, 10000);
	//vec2 trap = vec2(abs(npos));

	float deformation = 0;

	/*float trap3 = 0;
	float trap4 = 0;
	float trap5 = 0;*/

	int i = maxIterations;
	for (; i > 0; i--)
	{
		modulo = length(z);


		angle = atan(z.y, z.x) * power;
		modulo = pow(modulo, power);

		deformation = (sin(2*modulo) + sin(5*modulo) - cos(0.9*modulo))/3;

		if (modulo > 2)
		{
			break;
		}
		z = npos + modulo * vec2(cos(angle), sin(angle));
		//z = complexTan(z);

		//trap.x = (max(z.x, trap.x) + sinh(trap.x))*0.5; // change break position
		//trap.y = (max(z.y, trap.y) - tanh(trap.y))*0.5;

		/*float dist = ((-param)*z.x+2*z.y)/length(z);
		//deformation = dist; // Mandelbrot through lines
		trap.x = min(trap.x, dist * dist);
		
		dist = ((-50)*z.x+64*z.y)/length(z);
		trap.y = min(trap.y, dist * dist);
		
		dist = min(((-448+(deformation*200))*z.x+-(490+(deformation*-300))*z.y), ((-112+deformation*150)*z.x)+(param*z.y))/length(z);
		trap.z = min(trap.z, dist * dist);*/

		
		/*//float len = length(z-(cursor1 + deformation));

		trap2.x = min(trap2.x, len * len);
		len = length(z-cursor2 - deformation);
		trap2.y = min(trap2.y, len * len);
		len = length(z-cursor3);
		trap2.z = min(trap2.z, len * len);*/
		
		float len = distFromCurve(z, deformation);

		trap2.x = min(trap2.x, len * len);
		len = distFromCurve(z, cursor1.x);
		trap2.y = min(trap2.y, len * len);
		len = distFromCurve(z, cursor2.x);
		trap2.z = min(trap2.z, len * len);


		/*trap3 += distance(z, vec2(param, cursor1.y));
		trap4 += distance(z, vec2(cursor2.x, param));
		trap5 += distance(z, cursor3);*/

		/*if (z.x < npos.x + zoom * 0.1 && z.x > npos.x - zoom * 0.1)
		{
			trap = z;
		}*/

		
		/*float epsilon = pow(0.001,zoom);
		if (z.x < npos.x + epsilon && z.x > npos.x - epsilon)
		{
			//trap = mix(z,trap, 0.99);

			trap = z;
		}*/
		
		
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
	
	//color = vec4(trap.xy, length(trap)*0.5, 1);
	//color = vec4(trap.xy, cos(angle), 1);
	//color = vec4((npos.x - trap.x)/2, (npos.y - trap.y)/2, (length(trap)-length(npos))/2, 1);
	//color = vec4((npos.x + trap.x)/2, (npos.y + trap.y)/2, (length(trap)+length(npos))/2, 1);
	//color = vec4(distance(z, trap), distance(z, trap)/2, distance(z, trap)/4, 1);
	
	//color = sqrt(sqrt(vec4(trap.xyz, 1)));
	color = sqrt(sqrt(vec4(trap2.xyz, 1)));
	//color = vec4(trap2.x, trap2.y, mix(mix(trap2.z, trap.z, 0.5), trap.x, 0.5), 1);
	//color = mix(vec4(trap.x, trap.x/2,trap.x*2, 1);
	//color = vec4(trap3/(maxIterations - i), trap4/(maxIterations - i), trap5/(maxIterations - i), 1);


	//color = mix(vec4(trap.xy, length(trap)*0.5, 1), GetColor(float(maxIterations - i)), 0.5);
	//color = GetColor(float(maxIterations - i));
	//color = vec4(distance(trap, vec2(real, imag)), 0, 0, 1);
};