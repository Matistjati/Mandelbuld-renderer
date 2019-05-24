#include "headers/Fractal.h"
#include "headers/Uniform.h"

Fractal::Fractal(Shader & explorationShader, Shader & renderShader, Uniform<glm::ivec2> screenSize) : explorationShader(explorationShader), renderShader(renderShader), screenSize(screenSize)
{
}
