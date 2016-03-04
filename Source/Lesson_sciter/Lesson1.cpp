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

HDC			hDC=NULL;		// Private GDI Device Context  // OpenGL渲染描述表句柄
HGLRC		hRC=NULL;		// Permanent Rendering Context  // 窗口着色描述表句柄
HWND		hWnd=NULL;		// Holds Our Window Handle	// 保存我们的窗口句柄
HINSTANCE	hInstance;		// Holds The Instance Of The Application	// 保存程序的实例

bool	keys[256];			// Array Used For The Keyboard Routine	// 保存键盘按键的数组
bool	active=TRUE;		// Window Active Flag Set To TRUE By Default		// 窗口的活动标志，缺省为TRUE
bool	fullscreen=TRUE;	// Fullscreen Flag Set To Fullscreen Mode By Default	// 全屏标志缺省，缺省设定成全屏模式

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc		// WndProc的定义

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window	// 重置OpenGL窗口大小
{
	if (height==0)										// Prevent A Divide By Zero By		// 防止被零除
	{
		height=1;										// Making Height Equal One		// 将Height设为1
	}

	glViewport(0,0,width,height);						// Reset The Current Viewport		// 重置当前的视口

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix		// 选择投影矩阵
	glLoadIdentity();									// Reset The Projection Matrix		// 重置投影矩阵


	// Calculate The Aspect Ratio Of The Window		// 设置视口的大小
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix		// 选择模型观察矩阵
	glLoadIdentity();									// Reset The Modelview Matrix		// 重置模型观察矩阵
}

int InitGL(GLvoid)										// All Setup For OpenGL Goes Here		// 此处开始对OpenGL进行所有设置
{
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading		// 启用阴影平滑
	glClearColor(1.0f, 0.0f, 0.0f, 0.0f);				// Red Background		// 红色背景
	glClearDepth(1.0f);									// Depth Buffer Setup		// 设置深度缓存
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing		// 启用深度测试
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do    // 所作深度测试的类型
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations		// 告诉系统对透视进行修正
	return TRUE;										// Initialization Went OK		// 初始化 OK
}

int DrawGLScene(GLvoid)									// Here's Where We Do All The Drawing		// 从这里开始进行所有的绘制
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer		// 清除屏幕和深度缓存
	glLoadIdentity();									// Reset The Current Modelview Matrix		// 重置当前的模型观察矩阵
	
	// ....

	return TRUE;										// Everything Went OK		//  一切 OK
}

GLvoid KillGLWindow(GLvoid)			// Properly Kill The Window		// 正常销毁窗口
{
	if (fullscreen)										// Are We In Fullscreen Mode?		// 我们处于全屏模式吗?
	{
		ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop		// 是的话，切换回桌面
		ShowCursor(TRUE);								// Show Mouse Pointer		// 显示鼠标指针
	}

	if (hRC)											// Do We Have A Rendering Context?		// 我们拥有OpenGL渲染描述表吗?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?	// 我们能否释放DC和RC描述表?
		{
			MessageBox(NULL,L"Release Of DC And RC Failed.", L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?		// 我们能否删除RC?
		{
			MessageBox(NULL, L"Release Rendering Context Failed.", L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;										// Set RC To NULL		// 将RC设为 NULL
	}

	if (hDC && !ReleaseDC(hWnd,hDC))					// Are We Able To Release The DC		// 我们能否释放 DC?
	{
		MessageBox(NULL, L"Release Device Context Failed.", L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										// Set DC To NULL		// 将 hWnd 设为 NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?	// 能否销毁窗口?
	{
		MessageBox(NULL, L"Could Not Release hWnd.", L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;										// Set hWnd To NULL	// 将 hWnd 设为 NULL
	}

	if (!UnregisterClass( ::SciterClassName(),hInstance))			// Are We Able To Unregister Class   // 能否注销类?
	{
		MessageBox(NULL, L"Could Not Unregister Class.", L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Set hInstance To NULL		// 将 hInstance 设为 NULL
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
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match		// 保存查找匹配的结果
	WNDCLASS	wc;						// Windows Class Structure		// 窗口类结构
	DWORD		dwExStyle;				// Window Extended Style		// 扩展窗口风格
	DWORD		dwStyle;				// Window Style		// 窗口风格
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values	// 取得矩形的左上角和右下角的坐标值
	WindowRect.left=(long)0;			// Set Left Value To 0		// 将Left   设为 0
	WindowRect.right=(long)width;		// Set Right Value To Requested Width		// 将Right  设为要求的宽度
	WindowRect.top=(long)0;				// Set Top Value To 0		// 将Top    设为 0
	WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height		// 将Bottom 设为要求的高度

	fullscreen=fullscreenflag;			// Set The Global Fullscreen Flag		// 设置全局全屏标志

	hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window		// 取得我们窗口的实例
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.		// 移动时重画，并为窗口取得DC
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages	// WndProc处理消息
	wc.cbClsExtra		= 0;									// No Extra Window Data		// 无额外窗口数据
	wc.cbWndExtra		= 0;									// No Extra Window Data		// 无额外窗口数据
	wc.hInstance		= hInstance;							// Set The Instance		// 设置实例
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon	// 装入缺省图标
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer	// 装入鼠标指针
	wc.hbrBackground	= NULL;									// No Background Required For GL	// GL不需要背景
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu		// 不需要菜单
	wc.lpszClassName	= ::SciterClassName(); //L"OpenGL";								// Set The Class Name		// 设定类名字

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class    // 尝试注册窗口类
	{
		MessageBox(NULL, L"Failed To Register The Window Class.", L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;											// Return FALSE	// 退出并返回FALSE
	}
	
	if (fullscreen)												// Attempt Fullscreen Mode?		// 要尝试全屏模式吗?
	{
		DEVMODE dmScreenSettings;								// Device Mode		// 设备模式
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared	// 确保内存清空为零
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure	// Devmode 结构的大小
		dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width		// 所选屏幕宽度
		dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height	// 所选屏幕高度
		dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel		// 每象素所选的色彩深度
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		// 尝试设置显示模式并返回结果。注: CDS_FULLSCREEN 移去了状态条。
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			// 若模式失败，提供两个选项：退出或在窗口内运行。
			if (MessageBox(NULL, L"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?", L"NeHe GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE		// 选择窗口模式(Fullscreen=FALSE)
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				// 弹出一个对话框，告诉用户程序结束
				MessageBox(NULL, L"Program Will Now Close.", L"ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;									// Return FALSE	//  退出并返回 FALSE
			}
		}
	}

	if (fullscreen)												// Are We Still In Fullscreen Mode?		// 仍处于全屏模式吗?
	{
		dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style	// 扩展窗体风格
		dwStyle=WS_POPUP;										// Windows Style	// 窗体风格
		//ShowCursor(FALSE);										// Hide Mouse Pointer	// 隐藏鼠标指针
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style	// 扩展窗体风格
		dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style	//  窗体风格
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size	// 调整窗口达到真正要求的大小

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
		CreateWindowEx(	dwExStyle,							// Extended Style For The Window	// 扩展窗体风格
								::SciterClassName(),//L"OpenGL",							// Class Name		// 类名字
								title,								// Window Title		// 窗口标题
								dwStyle |							// Defined Window Style		// 选择的窗体属性
								WS_CLIPSIBLINGS |					// Required Window Style		// 必须的窗体风格属性
								WS_CLIPCHILDREN,					// Required Window Style		// 必须的窗体风格属性
								0, 0,								// Window Position		// 窗口位置
								WindowRect.right-WindowRect.left,	// Calculate Window Width		// 计算调整好的窗口宽度
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height		// 计算调整好的窗口高度
								NULL,								// No Parent Window		// 无父窗口
								NULL,								// No Menu		// 无菜单
								hInstance,							// Instance		// 实例
								NULL)
				*/				
								
								
								))								// Dont Pass Anything To WM_CREATE		// 不向WM_CREATE传递任何东东
	{
		KillGLWindow();								// Reset The Display		// 重置显示区
		MessageBox(NULL,L"Window Creation Error.", L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE		// 返回 FALSE
	}





	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be  // /pfd 告诉窗口我们所希望的东东，即窗口使用的像素格式
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor	// 上述格式描述符的大小
		1,											// Version Number		// 版本号
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window		// 格式支持窗口
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL		// 格式必须支持OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering	// 必须支持双缓冲
		PFD_TYPE_RGBA,								// Request An RGBA Format		// 申请 RGBA 格式
		bits,										// Select Our Color Depth		// 选定色彩深度
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored		// 忽略的色彩位
		0,											// No Alpha Buffer		// 无Alpha缓存
		0,											// Shift Bit Ignored		// 忽略Shift Bit
		0,											// No Accumulation Buffer		// 无累加缓存
		0, 0, 0, 0,									// Accumulation Bits Ignored		// 忽略聚集位
		16,											// 16Bit Z-Buffer (Depth Buffer)	// 16位 Z-缓存 (深度缓存)
		0,											// No Stencil Buffer		// 无蒙板缓存
		0,											// No Auxiliary Buffer		// 无辅助缓存
		PFD_MAIN_PLANE,								// Main Drawing Layer		// 主绘图层
		0,											// Reserved		// Reserved
		0, 0, 0										// Layer Masks Ignored		// 忽略层遮罩
	};
	
	if (!(hDC=GetDC(hWnd)))							// Did We Get A Device Context?		// 取得设备描述表了么?
	{
		KillGLWindow();								// Reset The Display		// 重置显示区
		MessageBox(NULL, L"Can't Create A GL Device Context.", L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE		// 返回 FALSE
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?	// Windows 找到相应的象素格式了吗?

	{
		KillGLWindow();								// Reset The Display		// 重置显示区
		MessageBox(NULL, L"Can't Find A Suitable PixelFormat.", L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE		// 返回 FALSE
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?		// 能够设置象素格式么?
	{
		KillGLWindow();								// Reset The Display		// 重置显示区
		MessageBox(NULL, L"Can't Set The PixelFormat.", L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE		// 返回 FALSE
	}

	if (!(hRC=wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?		// 能否取得着色描述表?
	{
		KillGLWindow();								// Reset The Display		// 重置显示区
		MessageBox(NULL, L"Can't Create A GL Rendering Context.", L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE		// 返回 FALSE
	}

	if(!wglMakeCurrent(hDC,hRC))					// Try To Activate The Rendering Context		// 尝试激活着色描述表
	{
		KillGLWindow();								// Reset The Display		// 重置显示区
		MessageBox(NULL, L"Can't Activate The GL Rendering Context.", L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		/return FALSE;								// Return FALSE		// 返回 FALSE
	}

	ShowWindow(hWnd,SW_SHOW);						// Show The Window		// 显示窗口
	SetForegroundWindow(hWnd);						// Slightly Higher Priority		// 略略提高优先级
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window		// 设置键盘的焦点至此窗口
	ReSizeGLScene(width, height);					// Set Up Our Perspective GL Screen		// 设置透视 GL 屏幕

	if (!InitGL())									// Initialize Our Newly Created GL Window		// 初始化新建的GL窗口
	{
		KillGLWindow();								// Reset The Display		// 重置显示区
		MessageBox(NULL, L"Initialization Failed.", L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE		// 返回 FALSE
	}


	::SciterSetCallback(hWnd,&SciterCallback,NULL);
	::SciterLoadFile(hWnd, GetUrl());
	::ShowWindow(hWnd, SW_SHOWNORMAL);

	return TRUE;									// Success		// 成功
}

LRESULT CALLBACK WndProc(	HWND	hWnd,			// Handle For This Window		// 窗口的句柄
							UINT	uMsg,			// Message For This Window		// 窗口的消息
							WPARAM	wParam,			// Additional Message Information		// 附加的消息内容
							LPARAM	lParam)			// Additional Message Information		// 附加的消息内容
{
	switch (uMsg)									// Check For Windows Messages		// 检查Windows消息
	{
		case WM_ACTIVATE:							// Watch For Window Activate Message		// 监视窗口激活消息
		{
			if (!HIWORD(wParam))					// Check Minimization State		// 检查最小化状态
			{
				active=TRUE;						// Program Is Active		// 程序处于激活状态
			}
			else
			{
				active=FALSE;						// Program Is No Longer Active		// 程序不再激活
			}

			return 0;								// Return To The Message Loop		// 返回消息循环
		}

		case WM_SYSCOMMAND:							// Intercept System Commands		// 系统中断命令
		{
			switch (wParam)							// Check System Calls		// 检查系统调用
			{
				case SC_SCREENSAVE:					// Screensaver Trying To Start?		// 屏保要运行?
				case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?		// 显示器要进入节电模式?
				return 0;							// Prevent From Happening		// 阻止发生
			}
			break;									// Exit		// 退出
		}

		case WM_CLOSE:								// Did We Receive A Close Message?		// 收到Close消息?
		{
			PostQuitMessage(0);						// Send A Quit Message		// 发出退出消息
			return 0;								// Jump Back		// 返回
		}

		case WM_KEYDOWN:							// Is A Key Being Held Down?		// 有键按下么?
		{
			keys[wParam] = TRUE;					// If So, Mark It As TRUE		// 如果是，设为TRUE
			return 0;								// Jump Back		// 返回
		}

		case WM_KEYUP:								// Has A Key Been Released?		// 有键放开么?
		{
			keys[wParam] = FALSE;					// If So, Mark It As FALSE		// 如果是，设为FALSE
			return 0;								// Jump Back		// 返回
		}

		case WM_SIZE:								// Resize The OpenGL Window		// 调整OpenGL窗口大小
		{
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));  // LoWord=Width, HiWord=Height		// LoWord=Width,HiWord=Height
			return 0;								// Jump Back		// 返回
		}
	}

	// Pass All Unhandled Messages To DefWindowProc
	// 向 DefWindowProc传递所有未处理的消息。
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

int WINAPI WinMain(	HINSTANCE	hInstance,			// Instance		// 当前窗口实例
					HINSTANCE	hPrevInstance,		// Previous Instance		// 前一个窗口实例
					LPSTR		lpCmdLine,			// Command Line Parameters		// 命令行参数
					int			nCmdShow)			// Window Show State		// 窗口显示状态
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


	//MSG		msg;									// Windows Message Structure		// Windowsx消息结构
	BOOL	done=FALSE;								// Bool Variable To Exit Loop	// 用来退出循环的Bool 变量

	// Ask The User Which Screen Mode They Prefer
	// 提示用户选择运行模式
	if (MessageBox(NULL, L"Would You Like To Run In Fullscreen Mode?", L"Start FullScreen?",MB_YESNO|MB_ICONQUESTION)==IDNO)
	{
		fullscreen=FALSE;							// Windowed Mode	// FALSE为窗口模式
	}

	// Create Our OpenGL Window
	// 创建OpenGL窗口
	if (!CreateGLWindow( L"NeHe's OpenGL Framework",640,480,16,fullscreen))
	{
		return 0;									// Quit If Window Was Not Created		// 失败退出
	}

	while(!done)									// Loop That Runs While done=FALSE		// 保持循环直到 done=TRUE
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Is There A Message Waiting?		// 有消息在等待吗?
		{
			if (msg.message==WM_QUIT)				// Have We Received A Quit Message?		// 收到退出消息?
			{
				done=TRUE;							// If So done=TRUE		// 是，则done=TRUE
			}
			else									// If Not, Deal With Window Messages	// 不是，处理窗口消息
			{
				TranslateMessage(&msg);				// Translate The Message		// 翻译消息
				DispatchMessage(&msg);				// Dispatch The Message		// 发送消息
			}
		}
		else										// If There Are No Messages		// 如果没有消息
		{
			// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
			// 绘制场景。监视ESC键和来自DrawGLScene()的退出消息
			if (active)								// Program Active?		// 程序激活的么?
			{
				if (keys[VK_ESCAPE])				// Was ESC Pressed?		// ESC 按下了么?
				{
					done=TRUE;						// ESC Signalled A Quit		// ESC 发出退出信号
				}
				else								// Not Time To Quit, Update Screen		// 不是退出的时候，刷新屏幕
				{
					DrawGLScene();					// Draw The Scene		// 绘制场景
					SwapBuffers(hDC);				// Swap Buffers (Double Buffering)		// 交换缓存 (双缓存)
				}
			}

			if (keys[VK_F1])						// Is F1 Being Pressed?		// F1键按下了么?
			{
				keys[VK_F1]=FALSE;					// If So Make Key FALSE		// 若是，使对应的Key数组中的值为 FALSE
				KillGLWindow();						// Kill Our Current Window		// 销毁当前的窗口
				fullscreen=!fullscreen;				// Toggle Fullscreen / Windowed Mode		// 切换 全屏 / 窗口 模式
				// Recreate Our OpenGL Window		// 重建 OpenGL 窗口
				if (!CreateGLWindow( L"NeHe's OpenGL Framework",640,480,16,fullscreen))
				{
					return 0;						// Quit If Window Was Not Created		// 如果窗口未能创建，程序退出
				}
			}
		}
	}

	// Shutdown		// 关闭程序
	KillGLWindow();									// Kill The Window		// 销毁窗口
	return (msg.wParam);							// Exit The Program		// 退出程序



}
	
