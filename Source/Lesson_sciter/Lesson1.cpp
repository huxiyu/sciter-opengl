/*
 *		This Code Was Created By Jeff Molofee 2000
 *		A HUGE Thanks To Fredric Echols For Cleaning Up
 *		And Optimizing This Code, Making It More Flexible!
 *		If You've Found This Code Useful, Please Let Me Know.
 *		Visit My Site At nehe.gamedev.net
 */

#include <windows.h>		// Header File For Windows
#include "sciter-x.h"

HINSTANCE ghInstance = 0;

// handle SC_LOAD_DATA requests - get data from resources of this application
UINT DoLoadData(LPSCN_LOAD_DATA pnmld)
{
  LPCBYTE pb = 0; UINT cb = 0;
  aux::wchars wu = aux::chars_of(pnmld->uri);
  if(wu.like(WSTR("res:*")))
  {
    // then by calling possibly overloaded load_resource_data method
    if(sciter::load_resource_data(ghInstance,wu.start+4, pb, cb))
      ::SciterDataReady( pnmld->hwnd, pnmld->uri, pb,  cb);
  } else if(wu.like(WSTR("this://app/*"))) {
    // try to get them from archive (if any, you need to call sciter::archive::open() first)
    aux::bytes adata = sciter::archive::instance().get(wu.start+11);
    if(adata.length)
      ::SciterDataReady( pnmld->hwnd, pnmld->uri, adata.start, adata.length);
  }
  return LOAD_OK;
}

// fulfill SC_ATTACH_BEHAVIOR request 
UINT DoAttachBehavior( LPSCN_ATTACH_BEHAVIOR lpab )
{
  sciter::event_handler *pb = sciter::behavior_factory::create(lpab->behaviorName, lpab->element);
  if(pb)
  {
    lpab->elementTag  = pb;
    lpab->elementProc = sciter::event_handler::element_proc;
    return TRUE;
  }
  return FALSE;
}

UINT SC_CALLBACK SciterCallback( LPSCITER_CALLBACK_NOTIFICATION pns, LPVOID callbackParam )
{
  // here are all notifiactions
  switch(pns->code) 
  {
    case SC_LOAD_DATA:          return DoLoadData((LPSCN_LOAD_DATA) pns);
    case SC_ATTACH_BEHAVIOR:    return DoAttachBehavior((LPSCN_ATTACH_BEHAVIOR)pns );
  }
  return 0;
}

LPCWSTR GetUrl() {
  static WCHAR url[MAX_PATH] = {0};

  wcscpy(url,L"file://");
  GetModuleFileName(NULL,url+7,MAX_PATH-7);
  WCHAR* t = wcsrchr(url,'\\');
  assert(t);
  wcscpy(t + 1,L"minimal.htm");
  return url;
}






#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library

HDC			hDC=NULL;		// Private GDI Device Context  // OpenGL��Ⱦ��������
HGLRC		hRC=NULL;		// Permanent Rendering Context  // ������ɫ��������
HWND		hWnd=NULL;		// Holds Our Window Handle	// �������ǵĴ��ھ��
HINSTANCE	hInstance;		// Holds The Instance Of The Application	// ��������ʵ��

bool	keys[256];			// Array Used For The Keyboard Routine	// ������̰���������
bool	active=TRUE;		// Window Active Flag Set To TRUE By Default		// ���ڵĻ��־��ȱʡΪTRUE
bool	fullscreen=TRUE;	// Fullscreen Flag Set To Fullscreen Mode By Default	// ȫ����־ȱʡ��ȱʡ�趨��ȫ��ģʽ

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc		// WndProc�Ķ���

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window	// ����OpenGL���ڴ�С
{
	if (height==0)										// Prevent A Divide By Zero By		// ��ֹ�����
	{
		height=1;										// Making Height Equal One		// ��Height��Ϊ1
	}

	glViewport(0,0,width,height);						// Reset The Current Viewport		// ���õ�ǰ���ӿ�

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix		// ѡ��ͶӰ����
	glLoadIdentity();									// Reset The Projection Matrix		// ����ͶӰ����


	// Calculate The Aspect Ratio Of The Window		// �����ӿڵĴ�С
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix		// ѡ��ģ�͹۲����
	glLoadIdentity();									// Reset The Modelview Matrix		// ����ģ�͹۲����
}

int InitGL(GLvoid)										// All Setup For OpenGL Goes Here		// �˴���ʼ��OpenGL������������
{
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading		// ������Ӱƽ��
	glClearColor(1.0f, 0.0f, 0.0f, 0.0f);				// Red Background		// ��ɫ����
	glClearDepth(1.0f);									// Depth Buffer Setup		// ������Ȼ���
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing		// ������Ȳ���
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do    // ������Ȳ��Ե�����
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations		// ����ϵͳ��͸�ӽ�������
	return TRUE;										// Initialization Went OK		// ��ʼ�� OK
}

int DrawGLScene(GLvoid)									// Here's Where We Do All The Drawing		// �����￪ʼ�������еĻ���
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer		// �����Ļ����Ȼ���
	glLoadIdentity();									// Reset The Current Modelview Matrix		// ���õ�ǰ��ģ�͹۲����
	
	// ....

	return TRUE;										// Everything Went OK		//  һ�� OK
}

GLvoid KillGLWindow(GLvoid)			// Properly Kill The Window		// �������ٴ���
{
	if (fullscreen)										// Are We In Fullscreen Mode?		// ���Ǵ���ȫ��ģʽ��?
	{
		ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop		// �ǵĻ����л�������
		ShowCursor(TRUE);								// Show Mouse Pointer		// ��ʾ���ָ��
	}

	if (hRC)											// Do We Have A Rendering Context?		// ����ӵ��OpenGL��Ⱦ��������?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?	// �����ܷ��ͷ�DC��RC������?
		{
			MessageBox(NULL,L"Release Of DC And RC Failed.", L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?		// �����ܷ�ɾ��RC?
		{
			MessageBox(NULL, L"Release Rendering Context Failed.", L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;										// Set RC To NULL		// ��RC��Ϊ NULL
	}

	if (hDC && !ReleaseDC(hWnd,hDC))					// Are We Able To Release The DC		// �����ܷ��ͷ� DC?
	{
		MessageBox(NULL, L"Release Device Context Failed.", L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										// Set DC To NULL		// �� hWnd ��Ϊ NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?	// �ܷ����ٴ���?
	{
		MessageBox(NULL, L"Could Not Release hWnd.", L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;										// Set hWnd To NULL	// �� hWnd ��Ϊ NULL
	}

	if (!UnregisterClass( ::SciterClassName(),hInstance))			// Are We Able To Unregister Class   // �ܷ�ע����?
	{
		MessageBox(NULL, L"Could Not Unregister Class.", L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Set hInstance To NULL		// �� hInstance ��Ϊ NULL
	}
}

/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
 *	title			- Title To Appear At The Top Of The Window				*
 *	width			- Width Of The GL Window Or Fullscreen Mode				*
 *	height			- Height Of The GL Window Or Fullscreen Mode			*
 *	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
 *	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	*/
 
BOOL CreateGLWindow(LPCWSTR title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match		// �������ƥ��Ľ��
	WNDCLASS	wc;						// Windows Class Structure		// ������ṹ
	DWORD		dwExStyle;				// Window Extended Style		// ��չ���ڷ��
	DWORD		dwStyle;				// Window Style		// ���ڷ��
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values	// ȡ�þ��ε����ϽǺ����½ǵ�����ֵ
	WindowRect.left=(long)0;			// Set Left Value To 0		// ��Left   ��Ϊ 0
	WindowRect.right=(long)width;		// Set Right Value To Requested Width		// ��Right  ��ΪҪ��Ŀ��
	WindowRect.top=(long)0;				// Set Top Value To 0		// ��Top    ��Ϊ 0
	WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height		// ��Bottom ��ΪҪ��ĸ߶�

	fullscreen=fullscreenflag;			// Set The Global Fullscreen Flag		// ����ȫ��ȫ����־

	hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window		// ȡ�����Ǵ��ڵ�ʵ��
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.		// �ƶ�ʱ�ػ�����Ϊ����ȡ��DC
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages	// WndProc������Ϣ
	wc.cbClsExtra		= 0;									// No Extra Window Data		// �޶��ⴰ������
	wc.cbWndExtra		= 0;									// No Extra Window Data		// �޶��ⴰ������
	wc.hInstance		= hInstance;							// Set The Instance		// ����ʵ��
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon	// װ��ȱʡͼ��
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer	// װ�����ָ��
	wc.hbrBackground	= NULL;									// No Background Required For GL	// GL����Ҫ����
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu		// ����Ҫ�˵�
	wc.lpszClassName	= ::SciterClassName(); //L"OpenGL";								// Set The Class Name		// �趨������

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class    // ����ע�ᴰ����
	{
		MessageBox(NULL, L"Failed To Register The Window Class.", L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;											// Return FALSE	// �˳�������FALSE
	}
	
	if (fullscreen)												// Attempt Fullscreen Mode?		// Ҫ����ȫ��ģʽ��?
	{
		DEVMODE dmScreenSettings;								// Device Mode		// �豸ģʽ
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared	// ȷ���ڴ����Ϊ��
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure	// Devmode �ṹ�Ĵ�С
		dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width		// ��ѡ��Ļ���
		dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height	// ��ѡ��Ļ�߶�
		dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel		// ÿ������ѡ��ɫ�����
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		// ����������ʾģʽ�����ؽ����ע: CDS_FULLSCREEN ��ȥ��״̬����
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			// ��ģʽʧ�ܣ��ṩ����ѡ��˳����ڴ��������С�
			if (MessageBox(NULL, L"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?", L"NeHe GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE		// ѡ�񴰿�ģʽ(Fullscreen=FALSE)
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				// ����һ���Ի��򣬸����û��������
				MessageBox(NULL, L"Program Will Now Close.", L"ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;									// Return FALSE	//  �˳������� FALSE
			}
		}
	}

	if (fullscreen)												// Are We Still In Fullscreen Mode?		// �Դ���ȫ��ģʽ��?
	{
		dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style	// ��չ������
		dwStyle=WS_POPUP;										// Windows Style	// ������
		//ShowCursor(FALSE);										// Hide Mouse Pointer	// �������ָ��
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style	// ��չ������
		dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style	//  ������
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size	// �������ڴﵽ����Ҫ��Ĵ�С

	/*
	HWND wnd = ::CreateWindowEx(
		0, 
		::SciterClassName(),
		L"Minimal Sciter Application",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0,
		800, 600,
		0,0,0,0);

  ::SciterSetCallback(wnd,&SciterCallback,NULL);
    
	::SciterLoadFile(wnd, GetUrl());
	::ShowWindow(wnd, SW_SHOWNORMAL);
	*/

	// Create The Window
	if (!(hWnd=
		
		 ::CreateWindowEx(
		0, 
		::SciterClassName(),
		L"Minimal Sciter Application",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0,
		800, 600,
		0,0,0,0)

		/*
		CreateWindowEx(	dwExStyle,							// Extended Style For The Window	// ��չ������
								::SciterClassName(),//L"OpenGL",							// Class Name		// ������
								title,								// Window Title		// ���ڱ���
								dwStyle |							// Defined Window Style		// ѡ��Ĵ�������
								WS_CLIPSIBLINGS |					// Required Window Style		// ����Ĵ���������
								WS_CLIPCHILDREN,					// Required Window Style		// ����Ĵ���������
								0, 0,								// Window Position		// ����λ��
								WindowRect.right-WindowRect.left,	// Calculate Window Width		// ��������õĴ��ڿ��
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height		// ��������õĴ��ڸ߶�
								NULL,								// No Parent Window		// �޸�����
								NULL,								// No Menu		// �޲˵�
								hInstance,							// Instance		// ʵ��
								NULL)
				*/				
								
								
								))								// Dont Pass Anything To WM_CREATE		// ����WM_CREATE�����κζ���
	{
		KillGLWindow();								// Reset The Display		// ������ʾ��
		MessageBox(NULL,L"Window Creation Error.", L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE		// ���� FALSE
	}





	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be  // /pfd ���ߴ���������ϣ���Ķ�����������ʹ�õ����ظ�ʽ
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor	// ������ʽ�������Ĵ�С
		1,											// Version Number		// �汾��
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window		// ��ʽ֧�ִ���
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL		// ��ʽ����֧��OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering	// ����֧��˫����
		PFD_TYPE_RGBA,								// Request An RGBA Format		// ���� RGBA ��ʽ
		bits,										// Select Our Color Depth		// ѡ��ɫ�����
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored		// ���Ե�ɫ��λ
		0,											// No Alpha Buffer		// ��Alpha����
		0,											// Shift Bit Ignored		// ����Shift Bit
		0,											// No Accumulation Buffer		// ���ۼӻ���
		0, 0, 0, 0,									// Accumulation Bits Ignored		// ���Ծۼ�λ
		16,											// 16Bit Z-Buffer (Depth Buffer)	// 16λ Z-���� (��Ȼ���)
		0,											// No Stencil Buffer		// ���ɰ建��
		0,											// No Auxiliary Buffer		// �޸�������
		PFD_MAIN_PLANE,								// Main Drawing Layer		// ����ͼ��
		0,											// Reserved		// Reserved
		0, 0, 0										// Layer Masks Ignored		// ���Բ�����
	};
	
	if (!(hDC=GetDC(hWnd)))							// Did We Get A Device Context?		// ȡ���豸��������ô?
	{
		KillGLWindow();								// Reset The Display		// ������ʾ��
		MessageBox(NULL, L"Can't Create A GL Device Context.", L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE		// ���� FALSE
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?	// Windows �ҵ���Ӧ�����ظ�ʽ����?

	{
		KillGLWindow();								// Reset The Display		// ������ʾ��
		MessageBox(NULL, L"Can't Find A Suitable PixelFormat.", L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE		// ���� FALSE
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?		// �ܹ��������ظ�ʽô?
	{
		KillGLWindow();								// Reset The Display		// ������ʾ��
		MessageBox(NULL, L"Can't Set The PixelFormat.", L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE		// ���� FALSE
	}

	if (!(hRC=wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?		// �ܷ�ȡ����ɫ������?
	{
		KillGLWindow();								// Reset The Display		// ������ʾ��
		MessageBox(NULL, L"Can't Create A GL Rendering Context.", L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE		// ���� FALSE
	}

	if(!wglMakeCurrent(hDC,hRC))					// Try To Activate The Rendering Context		// ���Լ�����ɫ������
	{
		KillGLWindow();								// Reset The Display		// ������ʾ��
		MessageBox(NULL, L"Can't Activate The GL Rendering Context.", L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		/return FALSE;								// Return FALSE		// ���� FALSE
	}

	ShowWindow(hWnd,SW_SHOW);						// Show The Window		// ��ʾ����
	SetForegroundWindow(hWnd);						// Slightly Higher Priority		// ����������ȼ�
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window		// ���ü��̵Ľ������˴���
	ReSizeGLScene(width, height);					// Set Up Our Perspective GL Screen		// ����͸�� GL ��Ļ

	if (!InitGL())									// Initialize Our Newly Created GL Window		// ��ʼ���½���GL����
	{
		KillGLWindow();								// Reset The Display		// ������ʾ��
		MessageBox(NULL, L"Initialization Failed.", L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE		// ���� FALSE
	}


	::SciterSetCallback(hWnd,&SciterCallback,NULL);
	::SciterLoadFile(hWnd, GetUrl());
	::ShowWindow(hWnd, SW_SHOWNORMAL);

	return TRUE;									// Success		// �ɹ�
}

LRESULT CALLBACK WndProc(	HWND	hWnd,			// Handle For This Window		// ���ڵľ��
							UINT	uMsg,			// Message For This Window		// ���ڵ���Ϣ
							WPARAM	wParam,			// Additional Message Information		// ���ӵ���Ϣ����
							LPARAM	lParam)			// Additional Message Information		// ���ӵ���Ϣ����
{
	switch (uMsg)									// Check For Windows Messages		// ���Windows��Ϣ
	{
		case WM_ACTIVATE:							// Watch For Window Activate Message		// ���Ӵ��ڼ�����Ϣ
		{
			if (!HIWORD(wParam))					// Check Minimization State		// �����С��״̬
			{
				active=TRUE;						// Program Is Active		// �����ڼ���״̬
			}
			else
			{
				active=FALSE;						// Program Is No Longer Active		// �����ټ���
			}

			return 0;								// Return To The Message Loop		// ������Ϣѭ��
		}

		case WM_SYSCOMMAND:							// Intercept System Commands		// ϵͳ�ж�����
		{
			switch (wParam)							// Check System Calls		// ���ϵͳ����
			{
				case SC_SCREENSAVE:					// Screensaver Trying To Start?		// ����Ҫ����?
				case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?		// ��ʾ��Ҫ����ڵ�ģʽ?
				return 0;							// Prevent From Happening		// ��ֹ����
			}
			break;									// Exit		// �˳�
		}

		case WM_CLOSE:								// Did We Receive A Close Message?		// �յ�Close��Ϣ?
		{
			PostQuitMessage(0);						// Send A Quit Message		// �����˳���Ϣ
			return 0;								// Jump Back		// ����
		}

		case WM_KEYDOWN:							// Is A Key Being Held Down?		// �м�����ô?
		{
			keys[wParam] = TRUE;					// If So, Mark It As TRUE		// ����ǣ���ΪTRUE
			return 0;								// Jump Back		// ����
		}

		case WM_KEYUP:								// Has A Key Been Released?		// �м��ſ�ô?
		{
			keys[wParam] = FALSE;					// If So, Mark It As FALSE		// ����ǣ���ΪFALSE
			return 0;								// Jump Back		// ����
		}

		case WM_SIZE:								// Resize The OpenGL Window		// ����OpenGL���ڴ�С
		{
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));  // LoWord=Width, HiWord=Height		// LoWord=Width,HiWord=Height
			return 0;								// Jump Back		// ����
		}
	}

	// Pass All Unhandled Messages To DefWindowProc
	// �� DefWindowProc��������δ�������Ϣ��
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

int WINAPI WinMain(	HINSTANCE	hInstance,			// Instance		// ��ǰ����ʵ��
					HINSTANCE	hPrevInstance,		// Previous Instance		// ǰһ������ʵ��
					LPSTR		lpCmdLine,			// Command Line Parameters		// �����в���
					int			nCmdShow)			// Window Show State		// ������ʾ״̬
{
	
	 ghInstance = hInstance;
  OleInitialize(NULL); // for shell interaction: drag-n-drop, etc.



/*  sciter::value undefined, map = sciter::value::from_string(aux::chars_of(L"{'0': 0, '1': 1, '2': 2, '3': 3}"), CVT_JSON_LITERAL);
  map.set_item(L"1", undefined);
  map.set_item(L"3", undefined);
  sciter::string s = map.to_string(CVT_JSON_LITERAL);
  s = s; */
	MSG msg;
  //while( ::IsWindow(wnd) && GetMessage(&msg, 0, 0, 0) )
	//{
	//	TranslateMessage(&msg);
	//	DispatchMessage(&msg);
	//}


	//MSG		msg;									// Windows Message Structure		// Windowsx��Ϣ�ṹ
	BOOL	done=FALSE;								// Bool Variable To Exit Loop	// �����˳�ѭ����Bool ����

	// Ask The User Which Screen Mode They Prefer
	// ��ʾ�û�ѡ������ģʽ
	if (MessageBox(NULL, L"Would You Like To Run In Fullscreen Mode?", L"Start FullScreen?",MB_YESNO|MB_ICONQUESTION)==IDNO)
	{
		fullscreen=FALSE;							// Windowed Mode	// FALSEΪ����ģʽ
	}

	// Create Our OpenGL Window
	// ����OpenGL����
	if (!CreateGLWindow( L"NeHe's OpenGL Framework",640,480,16,fullscreen))
	{
		return 0;									// Quit If Window Was Not Created		// ʧ���˳�
	}

	while(!done)									// Loop That Runs While done=FALSE		// ����ѭ��ֱ�� done=TRUE
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Is There A Message Waiting?		// ����Ϣ�ڵȴ���?
		{
			if (msg.message==WM_QUIT)				// Have We Received A Quit Message?		// �յ��˳���Ϣ?
			{
				done=TRUE;							// If So done=TRUE		// �ǣ���done=TRUE
			}
			else									// If Not, Deal With Window Messages	// ���ǣ���������Ϣ
			{
				TranslateMessage(&msg);				// Translate The Message		// ������Ϣ
				DispatchMessage(&msg);				// Dispatch The Message		// ������Ϣ
			}
		}
		else										// If There Are No Messages		// ���û����Ϣ
		{
			// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
			// ���Ƴ���������ESC��������DrawGLScene()���˳���Ϣ
			if (active)								// Program Active?		// ���򼤻��ô?
			{
				if (keys[VK_ESCAPE])				// Was ESC Pressed?		// ESC ������ô?
				{
					done=TRUE;						// ESC Signalled A Quit		// ESC �����˳��ź�
				}
				else								// Not Time To Quit, Update Screen		// �����˳���ʱ��ˢ����Ļ
				{
					DrawGLScene();					// Draw The Scene		// ���Ƴ���
					SwapBuffers(hDC);				// Swap Buffers (Double Buffering)		// �������� (˫����)
				}
			}

			if (keys[VK_F1])						// Is F1 Being Pressed?		// F1��������ô?
			{
				keys[VK_F1]=FALSE;					// If So Make Key FALSE		// ���ǣ�ʹ��Ӧ��Key�����е�ֵΪ FALSE
				KillGLWindow();						// Kill Our Current Window		// ���ٵ�ǰ�Ĵ���
				fullscreen=!fullscreen;				// Toggle Fullscreen / Windowed Mode		// �л� ȫ�� / ���� ģʽ
				// Recreate Our OpenGL Window		// �ؽ� OpenGL ����
				if (!CreateGLWindow( L"NeHe's OpenGL Framework",640,480,16,fullscreen))
				{
					return 0;						// Quit If Window Was Not Created		// �������δ�ܴ����������˳�
				}
			}
		}
	}

	// Shutdown		// �رճ���
	KillGLWindow();									// Kill The Window		// ���ٴ���
	return (msg.wParam);							// Exit The Program		// �˳�����



}
	
