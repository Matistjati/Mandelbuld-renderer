<traceIterationPercent>
#define LinneaRetarded 0
float trace(Ray ray, out vec4 trapOut, float px, out float percentSteps)
	{
		float res = -1;
		vec4 trap;

		float t = 0;
		float h = 0;
		int i = 0;
		for(; i<maxSteps; i++)
		{ 
			vec3 pos = ray.origin + ray.dir * t;
			h = sceneDistance(pos, trap);
			float th = 0.25 * px * t;

			if(h < th || h > maxDist)
			{
				break;
			}
			t += h;
		}

		trapOut = trap;
		percentSteps = float(i)/float(maxSteps);

		return res;
	}
</traceIterationPercent>