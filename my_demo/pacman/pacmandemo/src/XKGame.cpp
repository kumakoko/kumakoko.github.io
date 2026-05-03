 /************************************************************************
 *	@file		：	PacManDemo/XKGame.cpp
 *	@brief		：	
 *
 *	@author		：	Xiong Xinke
 *	@copyright	：	Xiong Xinke
 *	@date		：	2005-2-22 11:00:10
 ************************************************************************/
#include "XKGame.h"
#include "XKError.h"
#include "XKGlobals.h"
#include <tchar.h>

XKGame*		g_pGame = NULL;
HWND		g_hGameWnd = NULL;
HINSTANCE	g_hGameInst = NULL;
bool		g_bWindowed = true;

XKGame::XKGame()
{
	m_hGameWnd = NULL;
	m_pD3D = NULL;
	m_pD3DDevice = NULL;
	m_pPacMaze = NULL;
	m_pPlayer = NULL;
	m_pGhost = NULL;
	m_pD3DXSprite = NULL;
	m_pTimer = NULL;
	g_pGame = this;
}

XKGame::~XKGame()
{
	CleanUp();
}

/**
 * @name  XKGame::GetSafeHWND
 * @brief     
 * @return    HWND 
 * @remark    
 **/
HWND XKGame::GetSafeHWND()
{
	return m_hGameWnd;
}

/**
 * @name  XKGame::SetHWND
 * @brief     
 * @return    void 
 * @param     HWND hWnd
 * @remark    
 **/
void XKGame::SetHWND( HWND hWnd )
{
	m_hGameWnd = hWnd;
}

/**
 * @name  XKGame::MsgProc
 * @brief     
 * @return    int 
 * @param      HWND hWnd
 * @param     UINT msg
 * @param     WPARAM wParam
 * @param     LPARAM lParam
 * @remark    
 **/
int XKGame::MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	PAINTSTRUCT ps;
	RECT		rect;
	HDC			hdc; 

	switch( msg )
	{
	case WM_CREATE:
		return 0;
	case WM_PAINT:
		hdc = BeginPaint( hWnd, &ps );
        GetClientRect( hWnd, &rect );
        EndPaint (hWnd, &ps) ;
        return 0 ;
	case WM_DESTROY:
		PostQuitMessage (0) ;
        return 0 ;
	}
	
	return DefWindowProc (hWnd, msg, wParam, lParam) ;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return g_pGame->MsgProc( hWnd , msg , wParam ,lParam );
}

/**
 * @name  XKGame::InitWindow
 * @brief     初始化窗口，得到窗口句柄
 * @return    void 
 * @param     HINSTANCE hInst
 * @remark    
 **/
void XKGame::InitWindow( HINSTANCE hInst , int iCmdShow )
{
	 static TCHAR szAppName[] = TEXT ("PacMan Demo") ;
     WNDCLASS     wndclass ;

     wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
     wndclass.lpfnWndProc   = WndProc ;
     wndclass.cbClsExtra    = 0 ;
     wndclass.cbWndExtra    = 0 ;
     wndclass.hInstance     = hInst ;
     wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
     wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
     wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
     wndclass.lpszMenuName  = NULL ;
     wndclass.lpszClassName = szAppName ;

     if (!RegisterClass (&wndclass))
     {
          MessageBox(NULL, TEXT ("这个程序需要在Windows NT环境下运行!"), 
                      szAppName, MB_ICONERROR) ;
     }
     
     m_hGameWnd = CreateWindow(szAppName,                  // window class name
                          _T("The PacMan game demo"), // window caption
                          WS_OVERLAPPEDWINDOW,        // window style
                          CW_USEDEFAULT,              // initial x position
                          CW_USEDEFAULT,              // initial y position
                          CW_USEDEFAULT,              // initial x size
                          CW_USEDEFAULT,              // initial y size
                          NULL,                       // parent window handle
                          NULL,                       // window menu handle
                          hInst,                  // program instance handle
                          NULL) ; 
	 
	 ShowWindow( m_hGameWnd, iCmdShow );
     UpdateWindow( m_hGameWnd ) ;
	 
	 RECT WndRect;
	 
	 const int WINDOW_WIDTH = 800;
	 const int WINDOW_HEIGHT = 600;
	 GetWindowRect( m_hGameWnd , &WndRect );
	 MoveWindow( m_hGameWnd , WndRect.left , WndRect.top
				, WINDOW_WIDTH ,WINDOW_HEIGHT,TRUE);
	 
	 InitD3D();
	 InitPacMaze();

	 m_pTimer = new XKTimer;

	 if( m_pTimer == NULL )
	 {
		 throw XKError("无法创建计时器",__FILE__,__LINE__);
	 }
}

int WINAPI WinMain(	HINSTANCE instanceHnd, HINSTANCE prevInstanceHnd, char* p_CmdLine, int numCmds)	
{
	MSG	msg;
	XKGame Game;

	Game.InitWindow( instanceHnd , numCmds );

	//使用异常处理机制
	try
	{
		while(true)
		{
			if( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
				{
					break;
				}
				
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				Game.Run();
			}
		}
	}
	catch( XKError e )
	{
		e.Notify();
	}
	catch( exception e )
	{
		MessageBox( g_pGame->GetSafeHWND() , e.what(), 
					_T("出现无法处理的异常，程序强制性退出！"),
					MB_OK | MB_ICONERROR );
	}
	return 0;
}

/**
 * @name  XKGame::Run
 * @brief     整个游戏的主运行函数
 * @return    void 
 * @remark    
 **/
void XKGame::Run()
{
	m_pTimer->Update();
	m_pPlayer->Update();
	m_pGhost->Update();
	
	m_pD3DDevice->Clear(0,NULL,D3DCLEAR_TARGET,
						D3DCOLOR_XRGB(255,0,255),
						1.0f , 0 );
	m_pD3DDevice->BeginScene();

	m_pPacMaze->Render2D();
	m_pPlayer->Render2D();
	m_pGhost->Render2D();
	
	m_pD3DDevice->EndScene();

	m_pD3DDevice->Present(NULL,NULL,NULL,NULL);
}

/**
 * @name  XKGame::InitD3D
 * @brief     初始化D3D，D3D device等
 * @return    void 
 * @remark    
 **/
void XKGame::InitD3D()
{
	m_pD3D = Direct3DCreate8( D3D_SDK_VERSION );

	if( m_pD3D == NULL )
	{
		throw XKError("无法创建Direct3D对象");
	}

	//获取当前的显示设置
	D3DDISPLAYMODE	d3ddm;
	HRESULT hr = m_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT , &d3ddm );

	if( FAILED(hr) )
	{
		throw XKError("无法获取当前的显示设置",__FILE__ , __LINE__ , hr );
	}

	//用于获取Direct3D Device
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp , sizeof(D3DPRESENT_PARAMETERS) );

	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferCount = 1; //使用单后台缓冲区
	d3dpp.BackBufferFormat = d3ddm.Format;
	//创建direct3d device
	hr = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT,
						  D3DDEVTYPE_HAL,
						  m_hGameWnd,
						  D3DCREATE_SOFTWARE_VERTEXPROCESSING,//利用软件来处理顶点（如果硬件不支持的话）
						  &d3dpp , &m_pD3DDevice );
	
	if( FAILED(hr) )
	{
		throw XKError("无法创建Direct3D Device对象",__FILE__,__LINE__,hr);
	}

	//创建ID3DXSprite接口
	hr = D3DXCreateSprite( m_pD3DDevice , &m_pD3DXSprite );

	if( FAILED(hr) )
	{
		throw XKError("无法创建Direct3D Sprite对象",__FILE__,__LINE__,hr);
	}
}

/**
 * @name  XKGame::CleanUp
 * @brief     释放所有的动态对象，如D3D D3D Device等等
 * @return    void 
 * @remark    
 **/
void XKGame::CleanUp()
{
	if( m_pTimer )
	{
		delete m_pTimer;
		m_pTimer = NULL;
	}

	if( m_pPacMaze )
	{
		delete m_pPacMaze;
		m_pPacMaze = NULL;
	}
	
	if( m_pPlayer )
	{
		delete m_pPlayer;
		m_pPlayer = NULL;
	}

	if( m_pGhost )
	{
		delete m_pGhost;
		m_pGhost = NULL;
	}
	
	if( m_pD3DXSprite )
	{
		m_pD3DXSprite->Release();
		m_pD3D = NULL;
	}

	if( m_pD3DDevice )
	{
		m_pD3DDevice->Release();
		m_pD3DDevice = NULL;
	}

	if( m_pD3D )
	{
		m_pD3D->Release();
		m_pD3D = NULL;
	}
}

/**
 * @name  XKGame::GetD3DXSprite
 * @brief     获取D3D 的sprite接口
 * @return    LPD3DXSPRITE 
 * @remark    
 **/
LPD3DXSPRITE XKGame::GetD3DXSprite()
{
	return m_pD3DXSprite;
}

/**
 * @name  XKGame::GetD3DDevice
 * @brief     
 * @return    LPDIRECT3DDEVICE8 
 * @remark    
 **/
LPDIRECT3DDEVICE8 XKGame::GetD3DDevice()
{
	return m_pD3DDevice;
}

/**
 * @name  XKGame::InitPacMaze
 * @brief     初始化游戏地图 pac maze
 * @return    void 
 * @remark    
 **/
void XKGame::InitPacMaze()
{
	m_pPacMaze = new XKPacMaze;

	m_pPlayer = new XKPlayer(m_pPacMaze);

	m_pGhost = new XKGhost(m_pPacMaze);
}

/**
 * @name  XKGame::GetGameTimer
 * @brief     获取游戏系统的定时器
 * @return    XKTimer* 
 * @remark    
 **/
XKTimer* XKGame::GetGameTimer()
{
	return m_pTimer;
}