#include "OpenGLContext.h"
#include <iostream>
#include <iomanip>
#include "OpenUtility\Template\CMat4x4.h"

/** 
Default constructor for the OpenGLContext class. At this stage it does nothing 
but you can put anything you want here. 
*/  
OpenGLContext::OpenGLContext(void) {  
	//this->p.fov = 30.0f;
	//this->p.ratio = 1.0f;
	//this->p.nearP = 1.0f;
	//this->p.farP = 100.0f;

	this->frust.height = 9.0f;
	this->frust.width = 16.0f;
	this->frust.nearP = 1.0f;
	this->frust.farP = 100.0f;
	this->frust.distEyes = 0.1f;
	this->frust.distScreen_BASE = 10.0f;
	this->frust.distScreen = 20.0f;
	this->frust.fov_BASE = 30.0f;
}  
  
/** 
Constructor for the OpenGLContext class which will create a context given a windows HWND. 
*/  
OpenGLContext::OpenGLContext(HWND hwnd) {  
	create30Context(hwnd); // Create a context given a HWND  
}

/** 
Destructor for our OpenGLContext class which will clean up our rendering context 
and release the device context from the current window. 
*/  
OpenGLContext::~OpenGLContext(void) {  
	wglMakeCurrent(hdc, 0); // Remove the rendering context from our device context  
	wglDeleteContext(hrc); // Delete our rendering context  
  
	ReleaseDC(hwnd, hdc); // Release the device context from our window 
}

/** 
create30Context creates an OpenGL context and attaches it to the window provided by 
the HWND. This method currently creates an OpenGL 3.2 context by default, but will default 
to an OpenGL 2.1 capable context if the OpenGL 3.2 context cannot be created. 
*/  
bool OpenGLContext::create30Context(HWND hwnd) {  

	this->hwnd = hwnd; // Set the HWND for our window  
  
	hdc = GetDC(hwnd); // Get the device context for our window  
  
	PIXELFORMATDESCRIPTOR pfd; // Create a new PIXELFORMATDESCRIPTOR (PFD)  
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR)); // Clear our  PFD  
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR); // Set the size of the PFD to the size of the class  
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_STEREO; // Enable double buffering, opengl support and drawing to a window  
	pfd.iPixelType = PFD_TYPE_RGBA; // Set our application to use RGBA pixels  
	pfd.cColorBits = 32; // Give us 32 bits of color information (the higher, the more colors)  
	pfd.cDepthBits = 32; // Give us 32 bits of depth information (the higher, the more depth levels)  
	pfd.iLayerType = PFD_MAIN_PLANE; // Set the layer of the PFD 

	int nPixelFormat = ChoosePixelFormat(hdc, &pfd); // Check if our PFD is valid and get a pixel format back  
	if (nPixelFormat == 0) // If it fails  
	return false;  
  
	bool bResult = SetPixelFormat(hdc, nPixelFormat, &pfd); // Try and set the pixel format based on our PFD  
	if (!bResult) return false; //If it fails

	HGLRC tempOpenGLContext = wglCreateContext(hdc); // Create an OpenGL 2.1 context for our device context  
	wglMakeCurrent(hdc, tempOpenGLContext); // Make the OpenGL 2.1 context current and active  
 
	GLenum error = glewInit(); // Enable GLEW  
	if (error != GLEW_OK) // If GLEW fails  
		return false;

	int attributes[] = {  
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3, // Set the MAJOR version of OpenGL to 3  
		WGL_CONTEXT_MINOR_VERSION_ARB, 3, // Set the MINOR version of OpenGL to 2  
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB, // Set our OpenGL context to be forward compatible  
		0  
	}; 

	if (wglewIsSupported("WGL_ARB_create_context") == 1) { // If the OpenGL 3.x context creation extension is available  
		hrc = wglCreateContextAttribsARB(hdc, NULL, attributes); // Create and OpenGL 3.x context based on the given attributes  
		wglMakeCurrent(NULL, NULL); // Remove the temporary context from being active  
		wglDeleteContext(tempOpenGLContext); // Delete the temporary OpenGL 2.1 context  
		wglMakeCurrent(hdc, hrc); // Make our OpenGL 3.0 context current  
	}  
	else {  
		hrc = tempOpenGLContext; // If we didn't have support for OpenGL 3.x and up, use the OpenGL 2.1 context  
	}

	int glVersion[2] = {-1, -1}; // Set some default values for the version  
	glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]); // Get back the OpenGL MAJOR version we are using  
	glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]); // Get back the OpenGL MAJOR version we are using  
  
	std::cout << "Using OpenGL: " << glVersion[0] << "." << glVersion[1] << std::endl; // Output which version of OpenGL we are using On Windows, you won’t get a console for a Win32 Application, but a nifty trick to get console output, is to open up Command Prompt, navigate to your directory with your executable file, and use something like: “program.exe > temp.txt”  

	return true; // We have successfully created a context, return true  
}

void OpenGLContext::reshapeWindow(int w, int h) {  
	//this->p.ratio = (float)w/h;
	this->frust.height = h;
	this->frust.width = w;
	glViewport(0,0,w,h);
} 

void OpenGLContext::renderScene(float cameraDist,int stereo)
{	
	// Effacement de l'ecran
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Mise à jour de la matrice ModelView en fonction de la distance de la camera
	this->updateCamPos(cameraDist,stereo);

	glUniform3f(attribToShader("ambient"), 0.8, 0.8, 0.8);
	glUniform3f(attribToShader("diffuse"), 0.05, 0.05, 0.05);
	glUniform3f(attribToShader("lightPos"), cameraDist, 0, 0);

	// Envoie des matrices au shader
	switch(stereo){
	case 1:
		glUniformMatrix4fv(Shaders->RenderingShader["projMatrix"], 1, false, this->projMatrixD);
		break;
	case -1:
		glUniformMatrix4fv(Shaders->RenderingShader["projMatrix"], 1, false, this->projMatrixG);
		break;
	default:
		glUniformMatrix4fv(Shaders->RenderingShader["projMatrix"], 1, false, this->projMatrix);
	}
	glUniformMatrix4fv(Shaders->RenderingShader["lookAt"], 1, false, this->lookAt);
}

void OpenGLContext::swapBuffer(){
	SwapBuffers(hdc);
}

void OpenGLContext::setupScene()	
{
	/*Shader shaderBasique("Shaders/couleurs_2D.vert", "Shaders/couleurs.frag");
    shaderBasique.initialiser();*/
	OpenUtility::CStream log;

	// Compilation du Shader
	Shaders=new SShaders;
	if (!Shaders->ShaderVertex.LoadFile("Shaders/Shaders/basique_2D.vert"))
		log=Shaders->ShaderVertex.GetLog();
	if (!Shaders->ShaderFragment.LoadFile("Shaders/Shaders/basique.frag"))
		log=Shaders->ShaderFragment.GetLog();
	if (!Shaders->RenderingShader.LinkProgram())
		log=Shaders->RenderingShader.GetLog();

	// Utilisation du Shader
	Shaders->RenderingShader.UseProgram();

	glEnable(GL_DEPTH_TEST);
	
	// Génération des matrices ModelView et Projection
	this->generateLookAt();
	this->generateProjMatrix();

	this->generateTranslMat();
	this->generateScaleMat();

	// Génération des vertex d'un cube
	this->generateCube();
}

void OpenGLContext::generateProjMatrix(){
	for (int i=0; i <16; i++){
		this->projMatrix[i] = 0;
		this->projMatrixD[i] = 0;
		this->projMatrixG[i] = 0;
	}

	this->updateFrustnum();

	float left, right, top, bottom;
	// Frustnum
	left = - (frust.side*frust.distScreen*frust.nearP) / (frust.distScreen);
	right = -left;
	top = left*frust.height/frust.width;
	bottom = - top;

	this->projMatrix[0] = 2.0f * frust.nearP / (right - left);
	this->projMatrix[2] = (right + left) / (right - left);
	this->projMatrix[5] = 2.0f * frust.nearP / (top - bottom);
	this->projMatrix[6] = (top + bottom) / (top - bottom);
	this->projMatrix[10] = (frust.farP + frust.nearP) / (frust.farP - frust.nearP);
	this->projMatrix[11] = - 2.0f * frust.farP * frust.nearP / (frust.farP - frust.nearP);
	this->projMatrix[14] = -1.0f;

	left = - ((frust.side*frust.distScreen-frust.distEyes/2.0)*frust.nearP) / (frust.distScreen);
	right = ((frust.side*frust.distScreen+frust.distEyes/2.0)*frust.nearP) / (frust.distScreen);
	//top = (right-left)/2.0*frust.height/frust.width;
	//bottom = - top;

	this->projMatrixD[0] = 2.0f * frust.nearP / (right - left);
	this->projMatrixD[2] = (right + left) / (right - left);
	this->projMatrixD[5] = 2.0f * frust.nearP / (top - bottom);
	this->projMatrixD[6] = (top + bottom) / (top - bottom);
	this->projMatrixD[10] = (frust.farP + frust.nearP) / (frust.farP - frust.nearP);
	this->projMatrixD[11] = - 2.0f * frust.farP * frust.nearP / (frust.farP - frust.nearP);
	this->projMatrixD[14] = -1.0f;

	left = - ((frust.side*frust.distScreen+frust.distEyes/2.0)*frust.nearP) / (frust.distScreen);
	right = ((frust.side*frust.distScreen-frust.distEyes/2.0)*frust.nearP) / (frust.distScreen);

	this->projMatrixG[0] = 2.0f * frust.nearP / (right - left);
	this->projMatrixG[2] = (right + left) / (right - left);
	this->projMatrixG[5] = 2.0f * frust.nearP / (top - bottom);
	this->projMatrixG[6] = (top + bottom) / (top - bottom);
	this->projMatrixG[10] = (frust.farP + frust.nearP) / (frust.farP - frust.nearP);
	this->projMatrixG[11] = - 2.0f * frust.farP * frust.nearP / (frust.farP - frust.nearP);
	this->projMatrixG[14] = -1.0f;


}

void OpenGLContext::generateLookAt(){
	// Right
	this->lookAt[0]  =  1.0;	//X1
	this->lookAt[4]  =  0.0;	//Y1
	this->lookAt[8]  =  0.0;	//Z1
	// Normal
	this->lookAt[1]  =  0.0;	//X2
	this->lookAt[5]  =  1.0;	//Y2
	this->lookAt[9]  =  0.0;	//Z2
	// Opposite of View Direction
	this->lookAt[2]  =  0.0;	//X3
	this->lookAt[6]  =  0.0;	//Y3
	this->lookAt[10] =  1.0;	//Z3
	// Camera Position
	this->lookAt[12]  =  0.0;	//X
	this->lookAt[13]  =  0.0;	//Y
	this->lookAt[14]  =  27.0;	//Z
	// Last Line
	this->lookAt[3]  =  0.0;
	this->lookAt[7]  =  0.0;
	this->lookAt[11] =  0.0;
	this->lookAt[15] =  1.0;
}

void OpenGLContext::generateCube(){
	SVertex VertexArray[48];
	// Face Avant
	VertexArray[0] = createSVertex(1,1,1, 1,0,0, 0,0);
	VertexArray[1] = createSVertex(1,1,-1, 1,0,0, 0,1);
	VertexArray[2] = createSVertex(1,-1,1, 1,0,0, 1,0);
	VertexArray[3] = createSVertex(1,-1,-1, 1,0,0, 1,1);
	// Face Arrière
	VertexArray[4] = createSVertex(-1,1,1, -1,0,0, 0,0);
	VertexArray[5] = createSVertex(-1,1,-1, -1,0,0, 0,1);
	VertexArray[6] = createSVertex(-1,-1,1, -1,0,0, 1,0);
	VertexArray[7] = createSVertex(-1,-1,-1, -1,0,0, 1,1);
	// Face Droite
	VertexArray[8] = createSVertex(1,1,1, 0,1,0, 0,0);
	VertexArray[9] = createSVertex(-1,1,1, 0,1,0, 0,1);
	VertexArray[10] = createSVertex(1,1,-1, 0,1,0, 1,0);
	VertexArray[11] = createSVertex(-1,1,-1, 0,1,0, 1,1);
	// Face Gauche
	VertexArray[12] = createSVertex(1,-1,1, 0,-1,0, 0,0);
	VertexArray[13] = createSVertex(-1,-1,1, 0,-1,0, 0,1);
	VertexArray[14] = createSVertex(1,-1,-1, 0,-1,0, 1,0);
	VertexArray[15] = createSVertex(-1,-1,-1, 0,-1,0, 1,1);
	// Face Haute
	VertexArray[16] = createSVertex(1,1,1, 0,0,1, 0,0);
	VertexArray[17] = createSVertex(-1,1,1, 0,0,1, 0,1);
	VertexArray[18] = createSVertex(1,-1,1, 0,0,1, 1,0);
	VertexArray[19] = createSVertex(-1,-1,1, 0,0,1, 1,1);
	// Face Basse
	VertexArray[20] = createSVertex(1,1,-1, 0,0,-1, 0,0);
	VertexArray[21] = createSVertex(-1,1,-1, 0,0,-1, 0,1);
	VertexArray[22] = createSVertex(1,-1,-1, 0,0,-1, 1,0);
	VertexArray[23] = createSVertex(-1,-1,-1, 0,0,-1, 1,1);
	// Pour les lignes
	// Face Avant
	VertexArray[24] = createSVertex(1,1,1, 1,0,0);
	VertexArray[25] = createSVertex(1,1,-1, 1,0,0);
	VertexArray[27] = createSVertex(1,-1,1, 1,0,0);
	VertexArray[26] = createSVertex(1,-1,-1, 1,0,0);
	// Face Arrière
	VertexArray[28] = createSVertex(-1,1,1, -1,0,0);
	VertexArray[29] = createSVertex(-1,1,-1, -1,0,0);
	VertexArray[31] = createSVertex(-1,-1,1, -1,0,0);
	VertexArray[30] = createSVertex(-1,-1,-1, -1,0,0);
	// Face Droite
	VertexArray[32] = createSVertex(1,1,1, 0,1,0);
	VertexArray[33] = createSVertex(-1,1,1, 0,1,0);
	VertexArray[35] = createSVertex(1,1,-1, 0,1,0);
	VertexArray[34] = createSVertex(-1,1,-1, 0,1,0);
	// Face Gauche
	VertexArray[36] = createSVertex(1,-1,1, 0,-1,0);
	VertexArray[37] = createSVertex(-1,-1,1, 0,-1,0);
	VertexArray[39] = createSVertex(1,-1,-1, 0,-1,0);
	VertexArray[38] = createSVertex(-1,-1,-1, 0,-1,0);
	// Face Haute
	VertexArray[40] = createSVertex(1,1,1, 0,0,1);
	VertexArray[41] = createSVertex(-1,1,1, 0,0,1);
	VertexArray[43] = createSVertex(1,-1,1, 0,0,1);
	VertexArray[42] = createSVertex(-1,-1,1, 0,0,1);
	// Face Basse
	VertexArray[44] = createSVertex(1,1,-1, 0,0,-1);
	VertexArray[45] = createSVertex(-1,1,-1, 0,0,-1);
	VertexArray[47] = createSVertex(1,-1,-1, 0,0,-1);
	VertexArray[46] = createSVertex(-1,-1,-1, 0,0,-1);

	glGenVertexArrays(1, &VBAbuffer);
	glBindVertexArray(VBAbuffer);

	glGenBuffers(1,&VBObuffer);
	glBindBuffer(GL_ARRAY_BUFFER,VBObuffer);

	glBufferData(GL_ARRAY_BUFFER,sizeof(VertexArray),VertexArray,GL_STATIC_DRAW);
	glVertexAttribPointer(attribToShader("position"),4, GL_FLOAT,GL_FALSE,sizeof(SVertex),(void*)offsetof(SVertex,position));
	glVertexAttribPointer(attribToShader("coord"),2, GL_FLOAT,GL_FALSE,sizeof(SVertex),(void*)offsetof(SVertex,coord));
	glVertexAttribPointer(attribToShader("norm"),3, GL_FLOAT,GL_FALSE,sizeof(SVertex),(void*)offsetof(SVertex,norm));
	glEnableVertexAttribArray(attribToShader("position"));
	glEnableVertexAttribArray(attribToShader("coord"));
	glEnableVertexAttribArray(attribToShader("norm"));
	glBindVertexArray(0);
}

void OpenGLContext::generateTranslMat(){
	for(int i=0;i<16;i++){
		translMat[i]=0;
	}
	translMat[0] = translMat[5] = translMat[10] = translMat[15] = 1;
}

void OpenGLContext::generateScaleMat(){
	for(int i=0;i<16;i++){
		ScalelMat[i]=0;
	}
	ScalelMat[0] = ScalelMat[5] = ScalelMat[10] = ScalelMat[15] = 1;
}

void OpenGLContext::drawCube(XYZf center){
	
	translMat[12]=2*center.x;
	translMat[13]=2*center.y;
	translMat[14]=2*center.z;
	glUniformMatrix4fv(attribToShader("translObjet"), 1, false, translMat);
	glUniformMatrix4fv(attribToShader("scaleMat"), 1, false, OpenUtility::CMat4x4<float>().GetMatrix());

//	this->drawLines(1);
	glBindVertexArray(VBAbuffer);
	glDrawArrays(GL_TRIANGLE_STRIP,0,4);
	glDrawArrays(GL_TRIANGLE_STRIP,4,4);
	glDrawArrays(GL_TRIANGLE_STRIP,8,4);
	glDrawArrays(GL_TRIANGLE_STRIP,12,4);
	glDrawArrays(GL_TRIANGLE_STRIP,16,4);
	glDrawArrays(GL_TRIANGLE_STRIP,20,4);
}

void OpenGLContext::drawLines(float size){
	ScalelMat[0] = size;
	ScalelMat[5] = size;
	ScalelMat[10] = size;
	glUniformMatrix4fv(attribToShader("scaleMat"), 1, false, ScalelMat);
	glBindVertexArray(VBAbuffer);
	glDrawArrays(GL_LINE_LOOP,24,4);
	glDrawArrays(GL_LINE_LOOP,28,4);
	glDrawArrays(GL_LINE_LOOP,32,4);
	glDrawArrays(GL_LINE_LOOP,36,4);
	glDrawArrays(GL_LINE_LOOP,40,4);
	glDrawArrays(GL_LINE_LOOP,44,4);
}

SVertex OpenGLContext::createSVertex(float x, float y, float z,float nx, float ny, float nz,float t0,float t1){
	SVertex vertex;
	vertex.position[0]=x;
	vertex.position[1]=y;
	vertex.position[2]=z;
	vertex.position[3]=1.0f;
	vertex.norm[0] = nx;
	vertex.norm[1] = ny;
	vertex.norm[2] = nz;
	vertex.coord[0]=t0;
	vertex.coord[1]=t1;
	return vertex;
}

void OpenGLContext::updateCamPos(float dist,int stereo){
	// Position
	this->lookAt[12] =  stereo*frust.distEyes;
	this->lookAt[14] =  -dist;
}

GLuint OpenGLContext::attribToShader(char* variable) {
	return Shaders->RenderingShader[variable];
}

void OpenGLContext::updateFrustnum(){
	this->frust.side = (this->frust.distScreen_BASE / this->frust.distScreen) * tan( this->frust.fov_BASE / 2.0f);
}

void OpenGLContext::CheckGl(const char *file,int line)
{
	GLenum err;

	if ((err=glGetError())!=GL_NO_ERROR)
	{
		OpenUtility::CStream errStr;
		switch(err)
		{
#ifdef GL_INVALID_ENUM
		case GL_INVALID_ENUM:errStr="Invalid enum";break;
#endif
#ifdef GL_INVALID_VALUE
		case GL_INVALID_VALUE:errStr="Invalid value";break;
#endif
#ifdef GL_INVALID_OPERATION
		case GL_INVALID_OPERATION:errStr="Invalid operation";break;
#endif
#ifdef GL_STACK_OVERFLOW
		case GL_STACK_OVERFLOW:errStr="Stack overflow";break;
#endif
#ifdef GL_STACK_UNDERFLOW
		case GL_STACK_UNDERFLOW:errStr="Stack underflow";break;
#endif
#ifdef GL_OUT_OF_MEMORY
		case GL_OUT_OF_MEMORY:errStr="Out of memory";break;
#endif
#ifdef GL_TABLE_TOO_LARGE
		case GL_TABLE_TOO_LARGE:errStr="Table size exceeded";break;
#endif
		default:errStr.Format("Unknow error 0x%08.8X",err);break;
		}
		std::cout << "OpenGl error" << std::endl << "\t" << file << " (line " << line << ")" << std::endl << "\t" << errStr << std::endl;
	}
}
