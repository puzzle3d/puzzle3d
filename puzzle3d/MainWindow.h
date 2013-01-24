#ifndef _MAINWINDOW
#define _MAINWNIDOW

#include <Windows.h>
#include <GL/glew.h>
#include <GL/wglew.h>

#include "Structures.h"
#include "OpenGLContext.h"

class MainWindow {

private:
	HINSTANCE hInstance;
	char* title;
	int windowWidth;
	int windowHeight;
	GLuint VBAbuffer, VBObuffer;
	SShaders* Shaders;
	OpenGLContext* context;
	WNDPROC WndProc;

protected:
	HGLRC hrc;	// Identifiant du contexte OpenGL
	HDC hdc;	// Device context  
	HWND hWnd;	// Identifiant de la fenètre

public:
	MainWindow(char *title, OpenGLContext* context);
	MainWindow(char *title, int width, int height, OpenGLContext* context);
	~MainWindow();
	void assignWndProc(WNDPROC proc);
	bool createWindow();
	void reshapeWindow(int width, int height);
};

#endif