<maxIterations>4</maxIterations>
<maxSteps>100</maxSteps>
<maxIterationsRelease>512</maxIterationsRelease>
<maxStepsRelease>5000</maxStepsRelease>
<antiAliasing>2</antiAliasing>

<maxDist>power*4</maxDist>
<maxDistRelease>power*10</maxDistRelease>

<innerRadius>power-1</innerRadius>
<outerRadius>power+1</outerRadius>
<scale>genericParameter</scale>
<foldingLimit>power</foldingLimit>

<uniforms>
	uniform float power = 1;
	uniform float genericParameter = 1;
</uniforms>

<deformation>
	<>,
	<w.x = sin(w.x);
	w.x = sinh(w.x)*sin(w.x);>,
			
	<w.z = cos(w.z)*sin(w.z);
	w.y = cos(w.y)*sin(w.y);>,

</deformation>

<distanceEstimator>
	void sphereFold(inout vec3 w, inout float dz, float r2)
	{
		//float minRadius2 = power - distance(w,sun);
		//float fixedRadius2 = power + distance(w, sun);
		//float minRadius2 = power - 1;
		//float fixedRadius2 = power + 1;
	
		float innerRadius = <innerRadius>;
		float outerRadius = <outerRadius>;

		if (r2 < innerRadius)
		{ 
			// linear inner scaling
			float temp = outerRadius / innerRadius;
			w *= temp;
			dz*= temp;
		}
		else if (r2 < outerRadius)
		{ 
			// this is the actual sphere inversion
			float temp = outerRadius / r2;
			w *= temp;
			dz*= temp;
		}
	}

	void boxFold(inout vec3 w, inout float dz)
	{
		float foldingLimit = <foldingLimit>;
		w = clamp(w, -foldingLimit, foldingLimit) * 2.0 - w;
	}

	float DistanceEstimator(vec3 w, out vec4 resColor, float _)
	{
		float Scale = <scale>;
		vec3 c = w;
		float dr = 1.0;
		float m;
		vec4 trap = vec4(0,0,0,0);
		for (int n = 0; n < 8; n++) 
		{
			boxFold(w,dr);
			m = dot(w,w);
			sphereFold(w,dr,m);
 		
			w=Scale*w + c;  // Scale & Translate
		
			<deformation>

			dr = dr*abs(Scale)+1.0;

			trap = max(trap, vec4(abs(w), m));
		}
		resColor = trap;

		return length(w)/abs(dr);
	}
</distanceEstimator>

<trace>
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

		if (t < <maxDist>)
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
</trace>

<color>
	<vec3(1, 0, 0.3)>,
	<vec3(1, 0, 1)>,
	<vec3(0.78, 0.5, 0.13)>,
	<vec3(0.9, 0.15, 0.5)>,
	<vec3(0.5, 0, 0.5)>,
	<vec3(0.5, 0.5, 0.5)>,
	<vec3(0.3, 0.3, 0.3)>,
	<vec3(0.5, 0.65, 0.15)>,
</color>

<coloring>
	col = mix(col, <color>, clamp(pow(trap.w,6.0),0.0,1.0));
	col *= 0.5;
</coloring>

<edgeGlow>
	col += <color> * steps * steps * 0.45; // Fog
	// If you only want edge, i suggest disabling the sun and sky gradient
</edgeGlow>
