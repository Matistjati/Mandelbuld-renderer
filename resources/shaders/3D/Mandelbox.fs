%maxIterations%4%/maxIterations%
%maxSteps%100%/maxSteps%
%maxIterationsRelease%512%/maxIterationsRelease%
%maxStepsRelease%5000%/maxStepsRelease%
%antiAliasing%2%/antiAliasing%

%maxDist%power*4%/maxDist%
%maxDistRelease%power*10%/maxDistRelease%


%uniforms%
uniform float power = 1;
uniform float genericParameter = 1;
%/uniforms%

%distanceEstimator%
void sphereFold(inout vec3 z, inout float dz, float r2)
{
	//float minRadius2 = power - distance(z,sun);
	//float fixedRadius2 = power + distance(z, sun);
	//float minRadius2 = power - 1;
	//float fixedRadius2 = power + 1;
	
	float minRadius2 = 0;
	float fixedRadius2 = 1;

	if (r2<minRadius2)
	{ 
		// linear inner scaling
		float temp = (fixedRadius2/minRadius2);
		z *= temp;
		dz*= temp;
	}
	else if (r2<fixedRadius2)
	{ 
		// this is the actual sphere inversion
		float temp =(fixedRadius2/r2);
		z *= temp;
		dz*= temp;
	}
}

void boxFold(inout vec3 z, inout float dz)
{
	float foldingLimit = 1;
	z = clamp(z, -foldingLimit, foldingLimit) * 2.0 - z;
}
float DistanceEstimator(vec3 z, out vec4 resColor, float _)
{
	float Scale = -1.7;
	vec3 c = z;
	float dr = 1.0;
	float m;
	vec4 trap = vec4(0,0,0,0);
	for (int n = 0; n < 8; n++) 
	{
		boxFold(z,dr);
		m = dot(z,z);
		sphereFold(z,dr,m);

		//boxFold(z,dr);     
		//sphereFold(z,dr);  
 		
        z=Scale*z + c;  // Scale & Translate
		


		//z.x = sin(z.x);
		//z.x = sinh(z.x)*sin(z.x);
		
		//z.z = cos(z.z)*sin(z.z);
		//z.y = cos(z.y)*sin(z.y);

        dr = dr*abs(Scale)+1.0;

		trap = max(trap, vec4(abs(z), m));
	}
	resColor = trap;

	return length(z)/abs(dr);
}
%/distanceEstimator%

%trace%
float trace(Ray ray, out vec4 trapOut, float px, out float percentSteps)
{
	float res = -1.0;

	vec4 trap;

	float t = 0;
	int i = 0;
	for(; i<maxSteps; i++)
    { 
        vec3 pos = ray.origin + ray.dir * t;
		float h = sceneDistance(pos, trap);
        float th = 0.25 * px * t;

		if(h<th || h>power*8)
		{
			break;
        }
		t += h;
    }

	percentSteps = float(i)/float(maxSteps);

	if (t < %maxDist%)
	{
	trapOut = trap;
        res = t;
	}
	else
	{
		res = -1;
	}

    return res;
}
%/trace%

%color%
<vec3(0.78, 0.5, 0.13)>,
<vec3(0.9, 0.15, 0.5)>,
<vec3(0.5, 0, 0.5)>,
<vec3(0.5, 0.5, 0.5)>,
<vec3(0.3, 0.3, 0.3)>,
<vec3(0.5, 0.65, 0.15)>,
<vec3(1, 0, 0.3)>,
<vec3(0.8, 0.2, 0.8)>,
%/color%

%coloring%
col = mix(col, %color%, clamp(pow(trap.w,6.0),0.0,1.0));
col *= 0.5;
%/coloring%

%edgeGlow%
col += %color% * steps * steps * 0.45; // Fog
// If you only want edge, i suggest disabling the sun and sky gradient
%/edgeGlow%
