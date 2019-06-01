%maxIterations%4%/maxIterations%
%maxSteps%60%/maxSteps%
%shadowSoftness%4%/shadowSoftness% // Higher = harder shadow
%maxIterationsRelease%512%/maxIterationsRelease%
%maxStepsRelease%1000%/maxStepsRelease%
%antiAliasing%3%/antiAliasing%

%uniforms%
uniform float power = 8;
uniform float genericParameter = 1;
%/uniforms%

%include%
boundingSphere
%/include%

%distanceEstimator%
void sphereFold(inout vec3 z, inout float dz)
{
	float minRadius2 = power/2;
	float fixedRadius2 = power * 2;
	float r2 = dot(z,z);
	if (r2<minRadius2) { 
		// linear inner scaling
		float temp = (fixedRadius2/minRadius2);
		z *= temp;
		dz*= temp;
	} else if (r2<fixedRadius2) { 
		// this is the actual sphere inversion
		float temp =(fixedRadius2/r2);
		z *= temp;
		dz*= temp;
	}
}

void boxFold(inout vec3 z, inout float dz) {
	float foldingLimit =power/2;
	z = clamp(z, -foldingLimit, foldingLimit) * 2.0 - z;
}

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
        w = start + pow(r, Power) * vec3(sin(theta) * sin(phi), cos(phi), cos(theta) * sin(phi));

		//boxFold(w,dz);
		//sphereFold(w,dz);   

		w.x = sin(w.x);
		w.x = sinh(w.x)*sin(w.x);
		//w.y = cos(w.y)*sin(w.y);
		//w.y = atan(w.y, w.z)/sin(w.x);
		
		//w.x = atan(w.y, w.x);
		
		//w.x = tan(w.x);
		//w.y = tan(w.y);
		//w.z = tan(w.z);

		
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
%/distanceEstimator%

%coloring%
col = vec3(0.01);
col = mix(col, vec3(0.54,0.3,0.07), clamp(trap.y,0.0,1.0)); // Inner
col = mix(col, vec3(0.02,0.4,0.30), clamp(trap.z*trap.z,0.0,1.0));
col = mix(col, vec3(0.15, 0.4, 0.04), clamp(pow(trap.w,6.0),0.0,1.0)); // Stripes
col *= 0.5;
%/coloring%

%trace%
float trace(Ray ray, out vec4 trapOut, float px, out float percentSteps)
{
    float res = -1.0;

    //bounding sphere
    vec2 dis = boundingSphere( vec4(0.0,0.0,0.0,1.25), ray.origin, ray.dir);
    if(dis.y < 0.0)
        return -1.0;

    dis.x = max(dis.x, 0.0);
    dis.y = min(dis.y, 10.0);

    // raymarch fractal distance field
	vec4 trap;

	float t = dis.x;
	int i = 0;
	for(; i<maxSteps; i++  )
    { 
        vec3 pos = ray.origin + ray.dir * t;
		float h = sceneDistance(pos, trap);
        float th = 0.25 * px * t;

		if(t>dis.y || h < th)
		{
			break;
        }
		t += h;
    }

	percentSteps = float(i) / maxSteps;

    if(t < dis.y)
    {
        trapOut = trap;
        res = t;
    }

    return res;
}
%/trace%

%edgeGlow%
col += vec3(0.556, 0.843, 0.415) * steps * steps;
%/edgeGlow%