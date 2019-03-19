#version 330 core

layout(location = 0) out vec4 color;

uniform int width = 1080;
uniform int height = 1080;
uniform float power = 8;
uniform float epsilon = 0.001;

uniform mat2 yawMatrix;
uniform mat2 pitchMatrix;
uniform float time;

uniform vec3 eye;

const int maxIterations = 100;
const float bailout = 1.15;
const float stepMultiplier = 0.6;

float DE(vec3 p)
{
	vec3 w = p;
    float m = dot(w,w);

    vec4 trap = vec4(abs(w),m);
	float dz = 1.0;
    
    
	for(int i = 0; i < 4; i++)
    {
#if 1
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
	//resColor = vec4(m,trap.yzw);

    return 0.25* log(m)*sqrt(m)/dz;
}

int trace(vec3 origin, vec3 ray)
{
	float t = 0.0f;

	int i = 0;
	for (; i < maxIterations; i++)
	{
		vec3 p = origin + ray * t;

		float distance = DE(p);
		
		if (distance < epsilon)
		{
			break;
		}

		t += distance * stepMultiplier;
	}

	return i;
}

void main()
{
	vec2 uv = (gl_FragCoord.xy / vec2(width, height));
	uv = uv * 2.0 - 1.0;

	//uv.x *= float(width) / height;

	vec3 direction = normalize(vec3(uv.xy, 1));

	direction.zy *= pitchMatrix;

	direction.xz *= yawMatrix;
	
	int iterations = trace(eye.zyx, direction.xyz);

	float grayScale = float(iterations) / float(maxIterations);
    color = vec4(grayScale, grayScale, grayScale, 1.0);
}
