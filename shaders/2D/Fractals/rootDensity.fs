<include>
	complexPow, intersection, hash, polynomial
</include>


<type>compute</type>
<renderFrequency>1</renderFrequency>
<render>rootDensityRender.fs</render>
<localSizeDimensions>2</localSizeDimensions>


<uniforms>
	/*<GuiHint>GuiType: slider, Name: Max Iterations, Parent:renderParams, Range: (0, 10)</GuiHint>*/
	uniform float maxIterations = 5;
	
	/*<GuiHint>GuiType: slider, Name: Min difference, Parent:renderParams, Range: (1e-7, 1e-3)</GuiHint>*/
	uniform float epsilon = 0.001;
	
	/*<GuiHint>GuiType: colorPicker, Name: Color A, Parent: color</GuiHint>*/
	uniform vec3 colA = vec3(0.44, 0, 1);

	/*<GuiHint>GuiType: colorPicker, Name: Color B, Parent: color</GuiHint>*/
	uniform vec3 colB = vec3(0.2, 0.9, 0.9);
	
	/*<GuiHint>GuiType: slider, Name: Color frequency, Parent: color, Range: (0, 20)</GuiHint>*/
	uniform float frequency = 1.8;

	/*<GuiHint>GuiType: slider, Name: coloring base, Parent: color, Range: (0, 10)</GuiHint>*/
	uniform float base = 2.70833;
	
	/*<GuiHint>GuiType: slider, Name: Imaginary component, Parent: renderParams, Range: (-100, 100)</GuiHint>*/
	uniform float imag = 0;
	
	/*<GuiHint>GuiType: slider, Name: Start point distance to edge, Parent: renderParams, Range: (1, 100000)</GuiHint>*/
	uniform float distanceToEdge = 1;
	
	/*<GuiHint>GuiType: slider, Name: Start point multiplication offset, Parent: renderParams, Range: (0, 6.2831)</GuiHint>*/
	uniform float angleMultiplier = 1;
	
	/*<GuiHint>GuiType: slider, Name: Start point offset, Parent: renderParams, Range: (0, 6.2831)</GuiHint>*/
	uniform float angleOffset = 0;
	
	/*<GuiHint>GuiType: slider, Name: coefficient size, Parent: renderParams, Range: (0, 1)</GuiHint>*/
	uniform float coefficientSize = 0.0;
	
	/*<GuiHint>GuiType: checkBox, Name: Render starting points, Parent: renderParams</GuiHint>*/
	uniform bool renderStartPoints = false;

	/*<GuiHint>GuiType: Slider, Name: Rendering Amount, Parent: renderParams, Range: (0.01, 1)</GuiHint>*/
	uniform float renderSize = 0.65;

	// The area in the complex plane we render
	// ((left edge, bottom edge), (right edge, top edge))
	/*<GuiHint>GuiType: Slider, Name: Render Area, Parent: renderParams, Range: (-10, 10)</GuiHint>*/
	uniform vec4 renderArea = vec4(-1, -1, 1, 1);
</uniforms>

<buffers>
/*<bufferType>mainBuffer</bufferType>*/
/*<shouldBeCleared>checkBox, resetFrame, onUniformChange: [maxIterations, position, renderArea, zoom]</shouldBeCleared>*/
layout(std430, binding = 0) buffer densityMap
{
	vec4 points[];
};

/*<bufferType>outputBuffer</bufferType>*/
layout(std430, binding = 1) buffer outputMap
{
	vec4 shaderOutput[];
};
</buffers>

<constants>
	// Compute shaders are weird, for some reason i need to shift x
	#define IndexPoints(X,Y) uint((X)+(Y)*screenSize.x)
	const int size = 24;
</constants>


<main>
	uint fragCoord = IndexPoints(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y);

	vec2 uv = gl_GlobalInvocationID.xy/screenSize.xy;
	
	if (gl_GlobalInvocationID.x < screenSize.x*(renderSize*renderSize*renderSize*renderSize) && gl_GlobalInvocationID.y < screenSize.y*(renderSize*renderSize*renderSize*renderSize))
	{
		float polyIndex = 0;
		const int degree = size-1;
		vec2 poly[size];
		vec2 roots[size-1];


		// Initializing the polynomial coefficients
		float t = 1/1.1;
		for (int i = 0; i < size; i++)
		{
			uint hash = intHash(intHash(abs(int(frame))+i*308703+intHash(gl_GlobalInvocationID.x))*intHash(gl_GlobalInvocationID.y));
			float coefficient = ((uint(hash) & 0xffffffffU)/float(0xffffffffU)>0.5) ? 1 : -1;

			poly[i] = vec2(coefficient, imag);
			polyIndex += max(coefficient,0)*pow(1+base,-float(i));
		}

		// Initial values for the roots, evenly distributed points amoung a circle centered at the origin
		vec2 oldRoots[size-1];
		float startAngle = 1/(float(size)-1)*6.28318530718;
		float offset = startAngle*0.25;
		for (int i = 0; i < size - 1; i++)
		{
			// A point very close to but not on the unit circle
			vec2 startPoint;
			// If distancetoedge is large and maxIterations=1 and, the fractal curves become separated
			float theta = startAngle*i*angleMultiplier+offset+angleOffset;
			float r = 1-(0.000001*distanceToEdge);

			roots[i] = pow(r,i)*vec2(cos(theta),sin(theta))*clickPosition;
			oldRoots[i] = roots[i];
		}
		
		vec2 leading = poly[0];
		for (int i = 0; i < size; i++)
		{
			poly[i] = cDiv(poly[i], leading);
		}

		
		if (!renderStartPoints)
		{
			for (int iteration = 0; iteration < maxIterations; iteration++)
			{
				for (int i = 0; i < size - 1; i++)
				{
					oldRoots[i] = roots[i];
				}
				bool done = true;

				for (int i = 0; i < size - 1; i++)
				{
					vec2 currentRoot = oldRoots[i];
					// Aberth ehrlich root finding
					// https://i.ytimg.com/vi/XIzCzfMDSzk/maxresdefault.jpg
					vec2 sum = vec2(0);
					for (int j = 0; j < size - 1; j++)
					{
						if (i == j)
						{
							continue;
						}

						vec2 a = currentRoot-oldRoots[j];
						sum += vec2(a.x,-a.y)/dot(a,a);
					}

					vec2 y = vec2(0);
					vec2 dy = vec2(0);
					for (int j = 0; j < size; j++)
					{
						dy = cMul(dy,currentRoot) + y;
						y = cMul(y,currentRoot) + poly[j];
					}


					// We want to compute dot(dy,dy). This can present serious rounding errors using floats, so we compute it using doubles
					// The (mat2(dy.x,-dy.y,dy.y,dy.x) * y) is computed using floats for speed and division using doubles, the result is then cast back to float
					dvec2 Ddy = dvec2(dy);
					vec2 w = vec2((mat2(dy.x,-dy.y,dy.y,dy.x) * y) / dot(Ddy,Ddy));

					vec2 c = vec2(1,0) - cMul(w,sum);
					vec2 d = cDiv(w,c);
					


					// Durand kerner
					/*vec2 product = vec2(1,0);
					for (int j = 0; j < size - 1; j++)
					{
						if (i == j)
						{
							continue;
						}

						vec2 a = oldRoots[i]-oldRoots[j];
						product = cMul(a,product);
					}

					vec2 y = vec2(0);
					for (int j = 0; j < size; j++)
					{
						y = cMul(y,oldRoots[i]) + poly[j];
					}

					vec2 d = cdDiv(y,product);*/


					if (dot(d, d) > epsilon*epsilon)
					{
						done = false;
					}
					roots[i] -= d;
				}

		
				if (done)
				{
					break;
				}
			}
		}
		
		vec2 midPoint = vec2(abs(renderArea.x)-abs(renderArea.z),abs(renderArea.y)-abs(renderArea.w))*0.5;
		vec4 area = (renderArea+midPoint.xyxy)*zoom-midPoint.xyxy;
		area += vec4(position.xyxy)*vec4(1,-1,1,-1);
		vec2 map = vec2(screenSize.xy/vec2(area.z-area.x,area.w-area.y));
		



		for (int i = 0; i < size - 1; i++)
		{
			vec2 root = roots[i];
			root.x*=screenSize.y/screenSize.x;

			if (length(root) > 3)
			{
				vec2 coord = vec2(0.,0.);

				int x = int(clamp((coord.x-area.x)*map.x,0,screenSize.x)-0.5);

				int y = int(screenSize.y-(coord.y-area.y)*map.y);
				int imageIndex = int(x + screenSize.x * (y + 0.5));

				points[imageIndex].xyz = vec3(
				((poly[0].x > 0) ? 0.5 : 0) + ((poly[1].x > 0) ? 0.25 : 0)+ ((poly[2].x > 0) ? 0.125 : 0)+ ((poly[3].x > 0) ? 0.0625 : 0)+ ((poly[4].x > 0) ? 0.03125 : 0),
				((poly[5].x > 0) ? 0.5 : 0) + ((poly[6].x > 0) ? 0.25 : 0)+ ((poly[7].x > 0) ? 0.125 : 0)+ ((poly[8].x > 0) ? 0.0625 : 0)+ ((poly[9].x > 0) ? 0.03125 : 0),
				((poly[10].x > 0) ? 0.5 : 0) + ((poly[11].x > 0) ? 0.25 : 0)+ ((poly[12].x > 0) ? 0.125 : 0)+ ((poly[13].x > 0) ? 0.0625 : 0))+ ((poly[14].x > 0) ? 0.03125 : 0)+ ((poly[15].x > 0) ? 0.015625 : 0);
			}

			if(!InsideBox(root, area))
			{
				continue;
			}
			float x = round(clamp((root.x-area.x)*map.x,0,screenSize.x)-0.5);

			float y = round(screenSize.y-(root.y-area.y)*map.y);
			int index = int(x + screenSize.x * (y + 0.5));
			points[index] += vec4((cos(colA + colB * polyIndex*100 * frequency) * -0.5 + 0.5)*10,0);
		}
		/*
		// Gpu debugging
		vec2 value = roots[0];
		value /= 5;
		float signum = ((value.x > 0) ? 0.5 : 0) + ((value.y > 0) ? 0.25 : 0);
		points[int(gl_GlobalInvocationID.x+gl_GlobalInvocationID.y*screenSize.x)] = vec4(abs(value.xy), signum, 1);
		*/
	}
</main>