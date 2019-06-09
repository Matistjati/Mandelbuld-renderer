<extraSections>
[innerRadius], [outerRadius], [foldingLimit], [scale]
</extraSections>

<maxIterations>4</maxIterations>
<maxSteps>100</maxSteps>
<maxIterationsRelease>512</maxIterationsRelease>
<maxStepsRelease>5000</maxStepsRelease>
<antiAliasing>2</antiAliasing>

<maxDist>power*2</maxDist>
<maxDistRelease>power*20</maxDistRelease>

<innerRadius>power-1</innerRadius> // power - distance(w, sun);
<outerRadius>power+1</outerRadius> // power + distance(w, sun);
<scale>genericParameter</scale>
<foldingLimit>power</foldingLimit>
<zoom>.01</zoom>

<uniforms>
	uniform float power = 1;
	uniform float genericParameter = 1;
</uniforms>

<deformation>
	<w.x = sin(w.x);
	w.x = sinh(w.x)*sin(w.x);>,
			
	<w.z = cos(w.z)*sin(w.z);
	w.y = cos(w.y)*sin(w.y);>,

</deformation>

<include>
boxFold, sphereFold, triplexPow
</include>

<distanceEstimator>
	float DistanceEstimator(vec3 w, out vec4 resColor, float _)
	{
		float Scale = <scale>;
		vec3 c = w;
		float dr = 1.0;
		float m;
		vec4 trap = vec4(0,0,0,0);

		for (int n = 0; n < 8; n++) 
		{
			boxFold(w, <foldingLimit>);
			m = dot(w, w);
			sphereFold(w, dr, m, <innerRadius>, <outerRadius>);
			
			boxFold(w, <foldingLimit>);
			m = dot(w, w);
			sphereFold(w, dr, m, <innerRadius>, <outerRadius>);

			w.y = sin(w.y);
			w = triplexPow(w, _, dr, m);
 			//w.x = asinh(w.x);

 			//w.y = sin(w.y)*sinh(w.y);
			//w.x = (w.y);

			w=Scale*w + c;  // Scale & Translate
			dr = dr*abs(Scale)+1.0;
		

			<deformation>

			trap = mix(trap, vec4(w*w, m), 0.5);
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

		return res;
	}
</trace>

<color>
	<vec3(1, 0, 0.3)>,
	<vec3(1, 0, 1)>,
	<vec3(0, 1, 1)>,
	<vec3(0, 1, 0)>,
	<vec3(1, 1, 1)>,
	<vec3(0.9, 0.8, 0.2)>,
	<vec3(0.78, 0.5, 0.13)>,
	<vec3(0.9, 0.15, 0.5)>,
	<vec3(0.5, 0, 0.5)>,
	<vec3(0.5, 0.5, 0.5)>,
	<vec3(0.3, 0.3, 0.3)>,
	<vec3(0.5, 0.65, 0.15)>,
	<vec3(0, 1, 0)>,
</color>

<coloring>
	col = vec3(0.1);
	col = mix(col, <color>, clamp(pow(trap.w,6.0), 0, 1));
	col *= vec3(0., 0.3, 0.1);
	col *= 2;
	//col *= steps;
	//col *= 1 - length(uv); // Flashlight
	//col = sqrt(col);
</coloring>

<edgeGlow>
	col += <color> * (steps * steps+0.25) * 0.45; // Fog
	// If you only want edge, i suggest disabling the sun and sky gradient
</edgeGlow>
