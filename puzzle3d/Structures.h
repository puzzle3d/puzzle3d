#ifndef DEF_STRUCTURE
#define DEF_STRUCTURE

#include <Windows.h>
#include "OpenUtility\Utility\3D\CShaderFile.h"
#include "OpenUtility\Utility\3D\CShaderProgram.h"

#define PI 3.14

struct SShaders {
	SShaders() :
		ShaderVertex(OpenUtility::CShaderFile::EVertexShader,"vertex shader"),
		ShaderFragment(OpenUtility::CShaderFile::EFragmentShader,"fragment shader")
	{
		RenderingShader.AddShaderFile(&ShaderVertex);
		RenderingShader.AddShaderFile(&ShaderFragment);
	}
	OpenUtility::CShaderProgram RenderingShader;
	OpenUtility::CShaderFile ShaderVertex,ShaderFragment;
};
	
struct SVertex {
	GLfloat position[4];
	GLfloat coord[2];
	GLfloat norm[3];
};

struct Perspective {
	float fov, ratio, nearP, farP;
};
struct Frustnum {
	float height, width, nearP, farP;
	float distScreen_BASE, distScreen, distEyes, fov_BASE, side;
};

#endif