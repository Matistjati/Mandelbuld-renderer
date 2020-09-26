<include>
	intersection, hash, polynomial
</include>


<type>compute</type>
<renderFrequency>1</renderFrequency>
<render>rootDensityRender.fs</render>
<localSizeDimensions>2</localSizeDimensions>


<uniforms>
	/*<GuiHint>GuiType: slider, Name: Max Iterations, Parent:renderParams, Range: (1, 30)</GuiHint>*/
	uniform float maxIterations = 20;
	
	/*<GuiHint>GuiType: slider, Name: Min difference, Parent:renderParams, Range: (0, 1)</GuiHint>*/
	uniform float epsilon = 0.001;
	
	/*<GuiHint>GuiType: colorPicker, Name: Color A, Parent: color</GuiHint>*/
	uniform vec3 colA = vec3(0.44, 0, 1);

	/*<GuiHint>GuiType: colorPicker, Name: Color B, Parent: color</GuiHint>*/
	uniform vec3 colB = vec3(0.2, 0.9, 0.9);

	// TODO: remove or reword
	/*<GuiHint>GuiType: slider, Name: Starting guess, Parent: renderParams, Range: (-2, 2)</GuiHint>*/
	uniform vec2 startGuess = vec2(1.694201337, 1.177013233960);
	
	/*<GuiHint>GuiType: slider, Name: Imaginary component, Parent: renderParams, Range: (-100, 100)</GuiHint>*/
	uniform float imag = 0;
	
	/*<GuiHint>GuiType: slider, Name: coefficient size, Parent: renderParams, Range: (0, 1)</GuiHint>*/
	uniform float coefficientSize = 0.0;

	/*<GuiHint>GuiType: Slider, Name: Rendering Amount, Parent: renderParams, Range: (0.01, 1)</GuiHint>*/
	uniform float renderSize = 0.5;

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
		float counter = 1;
		int polynomialDegree = size-1;
		vec2 poly[size];
		vec2 roots[size-1];
		for (int i = 0; i < size - 1; i++)
		{
			roots[i] = vec2(0);
		}
		for (int i = 0; i < size; i++)
		{
			uint hash = intHash(intHash(abs(int(frame))+i*308703+intHash(gl_GlobalInvocationID.x))*intHash(gl_GlobalInvocationID.y));
			//float theta = abs(fract(sin(hash)*62758.5453123));
			//float theta = i*float(gl_GlobalInvocationID.x)*float(gl_GlobalInvocationID.y)*time;
			//float r = i*float(gl_GlobalInvocationID.x)*float(gl_GlobalInvocationID.y)*time;

			//(theta > 0.5) ? 1 : -1,0
			float coefficient = cos(3.141592*float(hash));
			coefficient = ((coefficient > 0) ? 1 : -1) + coefficient * coefficientSize;
			poly[i] = vec2(coefficient, imag);
			polyIndex += coefficient;
			counter *= 2;
		}
		/*
		for (int i = 0; i < size; i++)
		{
			poly[i] = vec2(0);
		}
		poly[0] = vec2(1,0);
		poly[size-1] = vec2(-1,0);
		*/
	
		int originalDegree = polynomialDegree;
		for (int i = 0; i < originalDegree; i++)
		{
			roots[i] = FindRoot(poly, polynomialDegree, uint(polyIndex));

			vec2 coefficient = poly[0];

			for (int j = 1; j < polynomialDegree; j++)
			{
				coefficient = mat2(coefficient,-coefficient.y,coefficient.x) * roots[i];
				coefficient += poly[j];
				poly[j] = coefficient;
			}


			polynomialDegree--;
		}

		vec2 midPoint = vec2(abs(renderArea.x)-abs(renderArea.z),abs(renderArea.y)-abs(renderArea.w))*0.5;
		vec4 area = (renderArea+midPoint.xyxy)*zoom-midPoint.xyxy;
		area += vec4(position.xyxy)*vec4(1,-1,1,-1);
		vec2 map = vec2(screenSize.xy/vec2(area.z-area.x,area.w-area.y));

		for (int i = 0; i < size - 1; i++)
		{
			vec2 root = mat2(0,-1,-1,0)*roots[i];

			if(!InsideBox(root, area))
			{
				continue;
			}
			float x = round(clamp((root.x-area.x)*map.x,0,screenSize.x)-0.5);

			float y = round(screenSize.y-(root.y-area.y)*map.y);
			int index = int(x + screenSize.x * (y + 0.5));
			points[index] += vec4((cos(colA + colB * polyIndex/17000*100) * -0.5 + 0.5)*10,0);
		}
		/*
		// Gpu debugging
		vec2 value = abs(roots[1]);
		value /= 1;
		float signum = ((value.x > 0) ? 0.5 : 0) + ((value.y > 0) ? 0.25 : 0);
		points[int(gl_GlobalInvocationID.x+gl_GlobalInvocationID.y*screenSize.x)] = vec4(abs(value.xy), signum, 1);
		*/
	}

	
</main>