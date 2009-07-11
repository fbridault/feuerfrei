#include <ilut.h>
#include "Scene/CDrawState.hpp"
#include "Shading/Glsl.hpp"
#include "Maths/CMatrix.hpp"

void initEngine()
{
	// Initialize DevIL library
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);

	// Create some singletons
	CDrawState::Create();
	CVisibilityState::Create();
	CShaderState::Create();
}
