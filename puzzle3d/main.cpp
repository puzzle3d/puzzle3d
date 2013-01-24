#include "OpenGLContext.h" 
#include "MainWindow.h"
#include "Shader.h"
#include "Monde.h"
#include "Statistique.h"
#include "OpenUtility\Template\CVector.h"
#include "OpenUtility\Utility\3D\CShaderFile.h"
#include "OpenUtility\Utility\3D\CShaderProgram.h"

#define CONSOLE 1
#define MOUSE_3D 1
#define FULL_SCREEN 1
#define WIIMOTE 1
#define EPSILON_WIIMOTE 0.05 //Pour 

#if CONSOLE
	#include <stdio.h>
	#include <io.h>
	#include <fcntl.h>
#endif

#if WIIMOTE
	#include "wiimote.h"
#endif

OpenGLContext openglContext;	// Variable stockant notre contexte OpenGL
Statistique stat;
int stereo = 1;

#if WIIMOTE
	wiimote remote;
	int last_left_right_input = (int)GetTickCount();
	int delta_left_right = 100; //100 millisecondes entre chaque input

	// in this demo we use a state-change callback to get notified of
	//  extension-related events, and polling for everything else
	// (note you don't have to use both, use whatever suits your app):

	void on_state_change (wiimote &remote, state_change_flags  changed, const wiimote_state &new_state)
	{
	// we use this callback to set report types etc. to respond to key events
	//  (like the wiimote connecting or extensions (dis)connecting).
	
	// NOTE: don't access the public state from the 'remote' object here, as it will
	//		  be out-of-date (it's only updated via RefreshState() calls, and these
	//		  are reserved for the main application so it can be sure the values
	//		  stay consistent between calls).  Instead query 'new_state' only.

	// the wiimote just connected
	if(changed & CONNECTED)
		{
		// ask the wiimote to report everything (using the 'non-continous updates'
		//  default mode - updates will be frequent anyway due to the acceleration/IR
		//  values changing):

		// note1: you don't need to set a report type for Balance Boards - the
		//		   library does it automatically.
		
		// note2: for wiimotes, the report mode that includes the extension data
		//		   unfortunately only reports the 'BASIC' IR info (ie. no dot sizes),
		//		   so let's choose the best mode based on the extension status:
		if(new_state.ExtensionType != wiimote::BALANCE_BOARD)
			{
			if(new_state.bExtension)
				remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR_EXT); // no IR dots
			else
				remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR);		//    IR dots
			}
		}
	// a MotionPlus was detected
	if(changed & MOTIONPLUS_DETECTED)
		{
		// enable it if there isn't a normal extension plugged into it
		// (MotionPlus devices don't report like normal extensions until
		//  enabled - and then, other extensions attached to it will no longer be
		//  reported (so disable the M+ when you want to access them again).
		if(remote.ExtensionType == wiimote_state::NONE) {
			bool res = remote.EnableMotionPlus();
			}
		}
	// an extension is connected to the MotionPlus
	else if(changed & MOTIONPLUS_EXTENSION_CONNECTED)
		{
		// We can't read it if the MotionPlus is currently enabled, so disable it:
		if(remote.MotionPlusEnabled())
			remote.DisableMotionPlus();
		}
	// an extension disconnected from the MotionPlus
	else if(changed & MOTIONPLUS_EXTENSION_DISCONNECTED)
		{
		// enable the MotionPlus data again:
		if(remote.MotionPlusConnected())
			remote.EnableMotionPlus();
		}
	// another extension was just connected:
	else if(changed & EXTENSION_CONNECTED)
		{
		// switch to a report mode that includes the extension data (we will
		//  loose the IR dot sizes)
		// note: there is no need to set report types for a Balance Board.
		if(!remote.IsBalanceBoard())
			remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR_EXT);
		}
	// extension was just disconnected:
	else if(changed & EXTENSION_DISCONNECTED)
		{
			// use a non-extension report mode (this gives us back the IR dot sizes)
		remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR);
		}
	}

#endif

#if MOUSE_3D
	#include <atlbase.h>
	#include <atlcom.h>
	#include <atlwin.h>
	#include <atltypes.h>
	#include <atlctl.h>
	#include <atlhost.h>
	#include <atlstr.h>
	using namespace ATL;
	#import "progid:TDxInput.Device.1" no_namespace
	
	CComPtr<ISensor> g3DSensor=0;
	CComPtr<IKeyboard> g3DKeyboard = 0;
	__int64 gKeyStates=0; 

	//Function
	void Poll3DxWare(Monde* monde)	{
		if (g3DSensor) {

			try	{
				CComPtr<IAngleAxis> pRotation = g3DSensor->Rotation;
				CComPtr<IVector3D> pTranslation = g3DSensor->Translation;

				// Check if the cap is still displaced
				if (pRotation->Angle > 0 || pTranslation->Length > 0 ) {

					int delta_translate = 10000;
					int delta_rotation = 100;

					if(!(pRotation->X == 0.0 && pRotation->Y == 0.0 && pRotation->Z == 0.0))
						(*monde).rotationPiece(pRotation->Angle/delta_rotation,XYZf(pRotation->X, -pRotation->Y, -pRotation->Z));
					

					float x = sqrt(abs(pTranslation->X))*pTranslation->X;
					float y = sqrt(abs(pTranslation->Y))*pTranslation->Y;
					float z = sqrt(abs(pTranslation->Z))*pTranslation->Z;

					(*monde).translatePiece(XYZf( (float) -x/delta_translate, (float) y/delta_translate, (float) z/delta_translate));
					
					stat.addMovementTimestamp((int)GetTickCount(), (*monde).getIndex());
					//stat.printObservation();

					/*
					printf("Angle : %f (%f;%f;%f)\tTranslation : %f(%f;%f;%f)\n",
								pRotation->Angle,
								pRotation->X/delta_translate,
								pRotation->Y/delta_translate,
								pRotation->Z/delta_translate,
								
								pTranslation->Length,
								pTranslation->X/delta_translate,
								pTranslation->Y/delta_translate,
								pTranslation->Z/delta_translate);//*/
					//Sleep(1000000);
					//camera.xcam+=pTranslation->X;
					//camera.ycam+=pTranslation->Y;
					//camera.zcam+=pTranslation->Z;*/
				}

				pRotation.Release();
				pTranslation.Release();

			} catch (...) {
				// Some sort of exception handling
			}
		}

		  if (g3DKeyboard) {   
				long nKeys = g3DKeyboard->Keys;  

				for (long i=1; i<=nKeys; i++) {   
					__int64 mask = (__int64)1<<(i-1);     
					if (g3DKeyboard->IsKeyDown(i) == VARIANT_TRUE)  {   
						if (!(gKeyStates & mask)) {   
							gKeyStates |= mask;
							if ( i == 1){
								(*monde).aimanter();
								if (!(*monde).getCollision()) {
									(*monde).changeSelectedPiece(1);
									stat.changePiece((int)GetTickCount());
								}
							}
							if ( i == 2){
								(*monde).aimanter();
								if (!(*monde).getCollision()) {
									(*monde).changeSelectedPiece(-1);
									stat.changePiece((int)GetTickCount());
								}
							}
						}
					} else {
						gKeyStates &= ~mask;   
					}
				}
		  }
  
	}

	void Init3dxWare() {
        HRESULT hr=::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED );
        if (!SUCCEEDED(hr)) {
                printf("Erreur d'initialisation ATL\n");
        } else {
                HRESULT hr;
                CComPtr<IUnknown> _3DxDevice;

                // Create the device object
                hr = _3DxDevice.CoCreateInstance(__uuidof(Device));
                if (SUCCEEDED(hr)) {
                        CComPtr<ISimpleDevice> _3DxSimpleDevice;

                        hr = _3DxDevice.QueryInterface(&_3DxSimpleDevice);
                        if (SUCCEEDED(hr)) {
                                // Get the interfaces to the sensor and the keyboard;
                                g3DSensor = _3DxSimpleDevice->Sensor;
								g3DKeyboard = _3DxSimpleDevice->Keyboard;
                                // Associate a configuration with this device
								//_3DxSimpleDevice->LoadPreferences(_T("Cube3DPolling"));
                                // Connect to the driver
                                _3DxSimpleDevice->Connect();
                        }
				}
        }
	}
#endif

#if FULL_SCREEN
	MainWindow window("Project",GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN),&openglContext);
#else 
	MainWindow window("Project",500,500,&openglContext);
#endif
  
float camera = 27.0f;
float oeil_offset = 1;
Monde monde(&openglContext, &stat);
bool running = true;	// Variable gérant l'arrêt du programme
  
HINSTANCE hInstance;	// Correspond à la HInstance de notre fenêtre dans le système Windows
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Message windows correspondant à l'interception d'un évènement

/** 
WndProc est la méthode standard pour gérer les messages et évènements en Win32.
Nous gérons ici les évènements de redimmensionnement de la fenètre poru en informer le contexte OpenGL.
*/  
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {  
	switch (message) {  
		case WM_SIZE:	// Si l'évènement est un redimmensionnage de la fenetre
		{  
			openglContext.reshapeWindow(LOWORD(lParam), HIWORD(lParam));	// On envoie la nouvelle taille au contexte OpenGL (pour le respect des proportions au moment de lier le rendu au bitmap de la fenetre)
			break;  
		}  
  
		case WM_DESTROY:	//Si l'évènement correspond à une demande de fermeture de la fenetre
		{  
			PostQuitMessage(0);  
			break;
		}  
	}  
  
	return DefWindowProc(hWnd, message, wParam, lParam);  
}  

int WINAPI WinMain(HINSTANCE	hInstance,  
					HINSTANCE	hPrevInstance,  
					LPSTR		lpCmdLine,  
					int			nCmdShow) {  
	MSG msg;

	//Afficher ou non la console => pour le débogage
	#if CONSOLE
		AllocConsole();
		HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE); int hCrt = _open_osfhandle((long) handle_out, _O_TEXT);	FILE* hf_out = _fdopen(hCrt, "w");setvbuf(hf_out, NULL, _IONBF, 1);*stdout = *hf_out;
		HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);hCrt = _open_osfhandle((long) handle_in, _O_TEXT);FILE* hf_in = _fdopen(hCrt, "r");setvbuf(hf_in, NULL, _IONBF, 128);*stdin = *hf_in;
	#endif

	#if MOUSE_3D
		Init3dxWare();
	#endif
	
	window.assignWndProc((WNDPROC)WndProc);
	window.createWindow();
	openglContext.setupScene();		// Initialisation de la scène OpenGL

	/** 
	Boucle principale, ne s'arrète que lorque "running" est à false
	*/ 
	//Variables pour le mouvement de la souris
	int x_middle =GetSystemMetrics(SM_CXSCREEN)/2,
			y_middle =GetSystemMetrics(SM_CYSCREEN)/2,
			x,y,
			x_mouse=x_middle,
			y_mouse=y_middle,
			old_x_mouse,
			old_y_mouse;
	float theta=0.25;
	int taille_bord = 250;


	stat.saveBeginTime();

	#if WIIMOTE
		while(!remote.Connect(wiimote::FIRST_AVAILABLE)) {}
		remote.ChangedCallback		= on_state_change;
		remote.EnableMotionPlus();
		remote.NunchukConnected();

		XYZf old_RotSpeed = XYZf(	remote.MotionPlus.Speed.Pitch,
									remote.MotionPlus.Speed.Roll,
									remote.MotionPlus.Speed.Yaw);
		XYZf rotSpeed = XYZf(	remote.MotionPlus.Speed.Pitch,
									remote.MotionPlus.Speed.Roll,
									remote.MotionPlus.Speed.Yaw);

		wiimote_state::motion_plus old_state, new_state;
		old_state = remote.MotionPlus;

	#endif

	while (running) {
		#if WIIMOTE
			remote.CallbackTriggerFlags = (state_change_flags)(CONNECTED | EXTENSION_CHANGED | MOTIONPLUS_CHANGED);

			int current_time = (int)GetTickCount();

			if( current_time - last_left_right_input > delta_left_right) {
				//Bouton droit
				if(remote.Button.Right()) {
					monde.aimanter();
					if (!monde.getCollision()) {
						monde.changeSelectedPiece(-1);
						stat.changePiece((int)GetTickCount());
					}
				}

				//Button gauche
				if(remote.Button.Left()) {
					monde.aimanter();
					if (!monde.getCollision()) {
						monde.changeSelectedPiece(1);
						stat.changePiece((int)GetTickCount());
					}
				}
				last_left_right_input = current_time;
			}

			//Nunchuk
			float x_val, y_val;
			if(remote.ExtensionType ==  wiimote_state::NUNCHUK) {
				if(remote.Nunchuk.C) { camera -= 0.2; }
				if(remote.Nunchuk.Z) { camera += 0.2; }


				x_val = 2*remote.Nunchuk.Joystick.X*abs(remote.Nunchuk.Joystick.X);
				y_val = 2*remote.Nunchuk.Joystick.Y*abs(remote.Nunchuk.Joystick.Y);
				cout << abs(remote.Nunchuk.Joystick.X) << " - " << abs(remote.Nunchuk.Joystick.Y) << endl;
				if(abs(remote.Nunchuk.Joystick.X) > EPSILON_WIIMOTE) monde.rotation(x_val,XYZf(0.0, 1.0, 0.0));
				if(abs(remote.Nunchuk.Joystick.Y) > EPSILON_WIIMOTE)	monde.rotation(y_val,XYZf(1.0, 0.0, 0.0));
			}

			
			new_state = remote.MotionPlus;
			float yaw = new_state.Speed.Yaw - old_state.Speed.Yaw;
			float pitch = new_state.Speed.Pitch - old_state.Speed.Pitch;
			float roll = new_state.Speed.Roll - old_state.Speed.Roll;

			float val;
			(abs(yaw)>abs(pitch))?val=yaw:val=pitch;
			(abs(val)>abs(roll))?val=val:val=roll;
			float angle = val;
			
			if((abs(yaw) > 2.0 || abs(roll) > 2.0 || abs(pitch) > 2.0)){
						monde.rotationPiece(abs(angle) ,XYZf( pitch, yaw, roll));
			}

			old_state = new_state;

			printf("Pitch:%8.3f deg  Roll:%8.3f deg  Yaw:%4f deg\n",
					yaw,
					roll, 
					pitch);

			//WiiMote
			
					

					/*float x = sqrt(abs(pTranslation->X))*pTranslation->X;
					float y = sqrt(abs(pTranslation->Y))*pTranslation->Y;
					float z = sqrt(abs(pTranslation->Z))*pTranslation->Z;

					monde).translatePiece(XYZf( (float) -x/delta_translate, (float) y/delta_translate, (float) z/delta_translate));
					stat.addMovementTimestamp((int)GetTickCount(), monde.getIndex());*/

		remote.RefreshState();


			

		#endif 
		if(!monde.getHasWon()) {

			Poll3DxWare(&monde);
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {		// attente de message (non blocante pour continuer à générer la scène lorsqu'il n'y a aucune interaction
				switch(msg.message){
				case WM_QUIT :
					running = false;
					break;
				case WM_KEYDOWN :
					switch(msg.wParam){
					case VK_ESCAPE:
						PostQuitMessage(0);
						break;
					case 'P':
						openglContext.incDistScreen();
						break;
					case 'M':
						openglContext.decDistScreen();
						break;
					case 'A':
						openglContext.incDistEye();
						break;
					case 'Z':
						openglContext.decDistEye();
						break;
					case 'G':
						monde.gagneTest();
						break;
					case 'S':
						stereo = 1-stereo;
						break;
					}
					break;

				case WM_MOUSEWHEEL :
					if(GET_WHEEL_DELTA_WPARAM(msg.wParam) < 0) { camera += 0.5; } else { camera -= 0.5;	}
					break;

				case WM_MOUSEMOVE:

					old_x_mouse = x_mouse;
					old_y_mouse = y_mouse;
					x_mouse = GET_X_LPARAM(msg.lParam);
					y_mouse = GET_Y_LPARAM(msg.lParam);

					y = y_mouse-old_y_mouse;
					x = x_mouse-old_x_mouse;

					stat.addMouseMove();

					monde.rotation(x*theta,XYZf(0.0, 1.0, 0.0));
					monde.rotation(y*theta,XYZf(1.0, 0.0, 0.0));

					if( x_mouse < taille_bord || x_mouse > (GetSystemMetrics(SM_CXSCREEN)-taille_bord) || y_mouse < taille_bord || y_mouse > GetSystemMetrics(SM_CYSCREEN)-taille_bord ) {
							SetCursorPos(x_middle, y_middle);
							x_mouse = x_middle;
							y_mouse = y_middle;
					}

					break;

				default :
					TranslateMessage(&msg);		// Converti le message Windows en un message interprétable par l'API
					DispatchMessage(&msg);		// On renvoie le mesage interprétable pour qu'il soit traité
				}
			}  
			else {	// Si aucun évènement à gérer, on déclenche le rendu
				// store time
				// annimate

				glDrawBuffer(GL_BACK_LEFT);
				openglContext.renderScene(camera,-stereo);
				monde.dessiner();
				glDrawBuffer(GL_BACK_RIGHT);
				openglContext.renderScene(camera,stereo);
				monde.dessiner();
				// sleep (objectif-(oldstore-time()))
				openglContext.swapBuffer();
			}
		  
		} else {
			running = false;
		}
	}

	stat.endData();

	////Ecran de fin
	//running = true;

	//while(running) {
	//	//Ecran de fin
	//}


	return (int) msg.wParam;  
}