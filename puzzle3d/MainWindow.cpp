#include "MainWindow.h"

MainWindow::MainWindow(char *title, OpenGLContext* context) {
	this->title = title;
	this->windowHeight = 500;
	this->windowWidth = 500;
	this->context = context;
}

MainWindow::MainWindow(char *title, int width, int height, OpenGLContext* context) {
	this->title = title;
	this->windowHeight = height;
	this->windowWidth = width;
	this->context = context;
}

MainWindow::~MainWindow() {
	wglMakeCurrent(hdc, 0); // Remove the rendering context from our device context  
	wglDeleteContext(hrc); // Delete our rendering context  
  
	ReleaseDC(hWnd, hdc); // Release the device context from our window 
//	delete(this->title);
}

bool MainWindow::createWindow() {
	WNDCLASS windowClass;	// Type de fenètre désiré
	DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;	// Options de la fenetre (support du Drag&Drop, premier plan etc)
  
	hInstance = GetModuleHandle(NULL);  // Retourne un identifiant de fenetre non utilisé
  
	// Paramétrage de la fenetre à créer
	windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;  
	windowClass.lpfnWndProc = (WNDPROC) WndProc;  
	windowClass.cbClsExtra = 0;  
	windowClass.cbWndExtra = 0;  
	windowClass.hInstance = hInstance;  
	windowClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);  
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);  
	windowClass.hbrBackground = NULL;  
	windowClass.lpszMenuName = NULL;  
	windowClass.lpszClassName = title;  
  
	// Enregistrement de notre classe dans le système
	if (!RegisterClass(&windowClass)) {  
		return false;  
	}

	// Création de la fenetre dans la boucle windows
	this->hWnd = CreateWindowEx(dwExStyle, title, title, WS_POPUP,  
							CW_USEDEFAULT, 0, windowWidth, windowHeight, NULL, NULL, hInstance, NULL);  
  
	this->context->create30Context(this->hWnd); // Création de notre contexte OpenGL dans notre fenetre
  
	ShowWindow(hWnd, SW_SHOW);  // Affichage de notre fenetre à l'écran
	UpdateWindow(hWnd); 
	ShowCursor(0);
  
	return true; 
}

void MainWindow::reshapeWindow(int width, int height) {
	this->windowHeight = height;
	this->windowWidth = width;
	this->context->reshapeWindow(width, height);
}

void MainWindow::assignWndProc(WNDPROC proc){
	this->WndProc = proc;
}