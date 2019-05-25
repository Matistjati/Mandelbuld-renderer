%uniforms%
uniform float power = 8;
uniform float genericParameter = 1;
uniform vec3 sun;
%/uniforms%

%constants%
const float antiAliasing = 2;
%/constants%

%DE%

float DistanceEstimator(vec3 start, out vec4 resColor, float Power)
{
	vec3 w = start;
    float m = dot(w,w);

    vec4 trap = vec4(abs(w),m);
	float dz = 1.0;
    
    
	for(int i = 0; i < maxIterations; i++)
    {
#if 0
        float m2 = m*m;
        float m4 = m2*m2;
		dz = Power*sqrt(m4*m2*m)*dz + 1.0;

        float x = w.x; float x2 = x*x; float x4 = x2*x2;
        float y = w.y; float y2 = y*y; float y4 = y2*y2;
        float z = w.z; float z2 = z*z; float z4 = z2*z2;

        float k3 = x2 + z2;
        float k2 = inversesqrt( pow(k3, Power - 1) );
        float k1 = x4 + y4 + z4 - 6.0*y2*z2 - 6.0*x2*y2 + 2.0*z2*x2;
        float k4 = x2 - y2 + z2;

        w.x = start.x +  64.0*x*y*z*(x2-z2)*k4*(x4-6.0*x2*z2+z4)*k1*k2;
        w.y = start.y + -16.0*y2*k3*k4*k4 + k1*k1;
        w.z = start.z +  -Power*y*k4*(x4*x4 - 28.0*x4*x2*z2 + 70.0*x4*z4 - 28.0*x2*z2*z4 + z4*z4)*k1*k2;
#else
        dz = (Power * pow(sqrt(m), Power - 1)) * dz + 1.0;
		//dz = Power*pow(m,(Power-1)*0.5)*dz + 1.0;
        
        float r = length(w);
        float theta = power * atan(w.x, w.z);
        float phi = power * acos(w.y / r);

		// Fun alternative: reverse sin and cos
        w = start + pow(r, Power) * vec3(sin(phi) * sin(theta), cos(phi), sin(phi) * cos(theta));
#endif
		//w = complexTan(w);
        trap = min(trap, vec4(abs(w),m));
		//vec2 len = distCurve(w, genericParameter);
        //trap = min(trap, vec4(abs(len), atan(len.y,len.x),m));

        m = dot(w,w);
		if( m > 256.0 )
            break;
	}
	
	resColor = vec4(m,trap.yzw);

    return 0.25* log(m)*sqrt(m)/dz;
}
%/DE%

%Color%
col = vec3(0.01);
col = mix(col, vec3(0.54,0.3,0.07), clamp(trap.y,0.0,1.0)); // Inner
col = mix(col, vec3(0.02,0.4,0.30), clamp(trap.z*trap.z,0.0,1.0));
col = mix(col, vec3(0.15, 0.4, 0.04), clamp(pow(trap.w,6.0),0.0,1.0)); // Stripes
col *= 0.5;
%/Color%

%main%
	vec2 uv = gl_FragCoord.xy / screenSize;
	uv = uv * 2.0 - 1.0;

	uv.x *= float(screenSize.x) / float(screenSize.y);

	vec3 direction = normalize(vec3(uv.xy, 1));

	direction.zy *= pitchMatrix;

	direction.xz *= yawMatrix;
	direction.xy *= rollMatrix;
	direction.y *= worldFlip;
	
	
	vec3 col = render(Ray(vec3(eye.z, eye.y * worldFlip, eye.x), direction.xyz));

    color = vec4(col.xyz, 1.0);
%/main%

%mainAA%
	vec2 uv = gl_FragCoord.xy / screenSize;
	uv = uv * 2.0 - 1.0;

	uv.x *= float(screenSize.x) / float(screenSize.y);

	vec3 direction = normalize(vec3(uv.xy, 1));

	direction.zy *= pitchMatrix;

	direction.xz *= yawMatrix;
	direction.xy *= rollMatrix;
	direction.y *= worldFlip;
	
	
	vec3 col = render(Ray(vec3(eye.z, eye.y * worldFlip, eye.x), direction.xyz));
	color = vec4(col.xyz, 1.0);
	/*
	vec3 col = vec3(0.0);
	for (int i = 0; i < antiAliasing; i++)
	{
		for (int j = 0; j < antiAliasing; j++)
		{
			vec2 frag = gl_FragCoord.xy;
			frag.x += float(i)/antiAliasing;
			frag.y += float(j)/antiAliasing;
			vec2 uv = frag / screenSize;
			uv = uv * 2.0 - 1.0;
			uv.x *= float(screenSize.x) / float(screenSize.y);

			vec3 direction = normalize(vec3(uv.xy, 1));

			direction.zy *= pitchMatrix;

			direction.xz *= yawMatrix;
			direction.xy *= rollMatrix;
			direction.y *= worldFlip;
	

			Ray	ray = Ray(vec3(eye.z, eye.y * worldFlip, eye.x), direction.xyz);
			col += render(ray);
	
			//vec3 col = render(Ray(vec3(eye.z, eye.y * worldFlip, eye.x), direction.xyz));
		}
	}
	col /= float(antiAliasing*antiAliasing);

    color = vec4(col.xyz, 1.0);*/
%/mainAA%