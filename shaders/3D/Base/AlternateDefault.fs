<traceIterationPercent>
float trace(Ray ray, out vec4 trapOut, float px, out float percentSteps)
	{
		vec4 trap;

		float t = 0;
		int i = 0;
		for(; i<maxSteps; i++)
		{ 
			vec3 pos = ray.origin + ray.dir * t;
			float h = sceneDistance(pos, trap);
			float th = 0.25 * px * t;

			if(h<th || th > maxDist)
			{
				break;
			}
			t += h;
		}

		percentSteps = float(i)/float(maxSteps);

		return -1;
	}
</traceIterationPercent>