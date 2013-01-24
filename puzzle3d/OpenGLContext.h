#pragma once

#include <Windows.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/wglew.h>
#include "Structures.h"
#include "Shader.h"
#include "OpenUtility\Template\CMat4x4.h"
#include "OpenUtility\Utility\3D\CShaderFile.h"
#include "OpenUtility\Utility\3D\CShaderProgram.h"
#include "OpenUtility\Utility\3D\3D.h"
#define GL_CHECK() OpenGLContext::CheckGl(__FILE__,__LINE__)

using namespace OpenUtility;


class OpenGLContext {
private:
	GLuint VBAbuffer,VBObuffer;
	SShaders* Shaders;
	SPivotf modelView;
	float projMatrix[16];
	float projMatrixD[16];
	float projMatrixG[16];
	float lookAt[16];
	float translMat[16];
	float ScalelMat[16];
	Frustnum frust;

public:
	OpenGLContext();	// Constructeur par défaut
	OpenGLContext(HWND hwnd);	// Constructeur créant le contexte OpenGL dans la fenetre dont on donne l'identifiant
	~OpenGLContext();	// Destructeur relachant la mémoire allouée à OpenGL
	bool create30Context(HWND hwnd);	// Création d'un contexte OpenGL 3.x
	void setupScene();	// Initialisation des paramètres de la scène
	void reshapeWindow(int w, int h);	// Mise à l'échelle du contexte (en fonction de la taille de la fenètre)
	void renderScene(float cameraDist,int stereo=0);		// Rendu de la scène
	void swapBuffer();
	void generateProjMatrix();
	void generateLookAt();
	void generateCube();
	void generateTranslMat();
	void generateScaleMat();
	void drawCube(XYZf center);
	void drawLines(float size);
	SVertex createSVertex(float x, float y, float z,float nx, float ny, float nz,float t0=0,float t1=0);
	void updateCamPos(float dist,int stereo);
	GLuint attribToShader(char* variable);
	static void CheckGl(const char *file,int line);
	inline void incDistEye(){this->frust.distEyes += 0.1f;generateProjMatrix();}
	inline void decDistEye(){((this->frust.distEyes-0.1f)>0.0f)?this->frust.distEyes -= 0.1f:false; generateProjMatrix();}
	inline void incDistScreen() {this->frust.distScreen += 0.5f;generateProjMatrix();}
	inline void decDistScreen() {((this->frust.distScreen-0.5f)>0.0f)?this->frust.distScreen -= 0.5f:false; generateProjMatrix();}
	void updateFrustnum();

protected:
	HGLRC hrc;	// Identifiant du contexte OpenGL
	HDC hdc;	// Device context  
	HWND hwnd;	// Identifiant de la fenètre
};