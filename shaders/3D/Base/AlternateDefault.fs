<traceNoMaxDist>
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

			if(h<th)
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
</traceNoMaxDist>