  /************************************************************************
 *	@file		：	ModelDemo/XKDemo.cpp
 *	@brief		：	
 *
 *	@author		：	Xiong Xinke
 *	@copyright	：	Xiong Xinke
 *	@date		：	2005-2-27 1:53:49
 ************************************************************************/

#include "XKGlobals.h"
#include "XKMD2Model.h"

HWND				g_hWnd = NULL;
LPDIRECT3D8			g_pD3D = NULL;
LPDIRECT3DDEVICE8	g_pD3DDevice = NULL;
LPDIRECT3DTEXTURE8	g_pMD2Texture = NULL;

D3DXMATRIX			g_matWorld;
XKMD2Model			g_MD2Model;
int					g_nFrame = 0;

/**
 * @name  WndProc
 * @brief     
 * @return    LRESULT CALLBACK 
 * @param      HWND hwnd
 * @param     UINT message
 * @param     WPARAM wparam
 * @param     LPARAM lparam
 * @remark    
 **/
LRESULT CALLBACK WndProc( HWND hWnd, UINT nMessage, 
						  WPARAM wParam, LPARAM lParam )
{
	switch( nMessage )
	{
	case WM_CLOSE:
		PostQuitMessage( 0 );
		return 0;
	case WM_CREATE:
		return 0;
	}
	return DefWindowProc( hWnd, nMessage, wParam, lParam );
}

/**
 * @name  InitWindow
 * @brief     初始化窗口
 * @return    bool 
 * @param     HINSTANCE hInst
 * @remark    
 **/
bool InitWindow( HINSTANCE hInst )
{
	WNDCLASS wndclass;
	wndclass.style         = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc   = WndProc;
	wndclass.cbClsExtra    = 0;
	wndclass.cbWndExtra    = 0;
	wndclass.hInstance     = hInst;
	wndclass.hIcon         = NULL;
	wndclass.hCursor       = LoadCursor( NULL, IDC_ARROW );
	wndclass.hbrBackground = NULL;
	wndclass.lpszMenuName  = NULL;
	wndclass.lpszClassName = _T("model demo");

	if( !RegisterClass( &wndclass ) )
		return false;

	g_hWnd = CreateWindow( _T("model demo"), 
						   _T("model demo"), 
						   WS_OVERLAPPEDWINDOW,
						   0, 0, 
						   WINDOW_WIDTH, 
						   WINDOW_HEIGHT, 
		NULL, NULL, hInst, NULL );

	if( NULL == g_hWnd )
		return false;

	UpdateWindow( g_hWnd );
	ShowWindow( g_hWnd, SW_NORMAL );
	SetCursor( NULL );

	return true;
}

/**
 * @name  InitD3D
 * @brief     初始化Driect3D
 * @return    bool 
 * @remark    
 **/
bool InitD3D()
{
	g_pD3D = Direct3DCreate8( D3D_SDK_VERSION );
        
	if( NULL == g_pD3D )
		return false;

	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.Windowed = FALSE;

	D3DDISPLAYMODE d3ddm;
	HRESULT result = g_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm );
	
	if( FAILED( result ) )
	{
		if( g_pD3D )
		{
			g_pD3D->Release();
			g_pD3D = NULL;
		}
		DestroyWindow( g_hWnd );
		g_hWnd = NULL;
		return false;	
	}

	d3dpp.BackBufferFormat = d3ddm.Format;
	d3dpp.BackBufferWidth  = WINDOW_WIDTH;
	d3dpp.BackBufferHeight = WINDOW_HEIGHT;
	d3dpp.BackBufferCount = 1;
	d3dpp.hDeviceWindow = g_hWnd;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.EnableAutoDepthStencil = TRUE;

	result = g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, 
		D3DDEVTYPE_HAL, g_hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pD3DDevice );

	if( FAILED( result ) )
	{
		if( g_pD3D )
		{
			g_pD3D->Release();
			g_pD3D = NULL;
		}
		
		DestroyWindow( g_hWnd );
		g_hWnd = NULL;
		
		return false;
	}

	return true;
}

/**
 * @name  InitMD2Model
 * @brief     
 * @return    bool 
 * @remark    
 **/
bool InitMD2Model( )
{
	if( 1 == g_MD2Model.LoadFile( MD2MODELFILE ) )
		return true;

	return false;
}
/**
 * @name  InitTextures
 * @brief     初始化各个模型所需的纹理纹理
 * @return    void 
 * @remark    
 **/
bool InitTextures()
{
	HRESULT hr;
	hr = D3DXCreateTextureFromFileEx(g_pD3DDevice,
						MD2MODELTEXTURENAME,
						0,0,0,0,	
						D3DFMT_UNKNOWN,
						D3DPOOL_MANAGED,
						D3DX_FILTER_TRIANGLE|D3DX_FILTER_DITHER,
						D3DX_FILTER_TRIANGLE|D3DX_FILTER_DITHER, 
						0,	0,	0,
						&g_pMD2Texture
					);

	if( FAILED(hr) )
	{
		if( g_pD3DDevice )
		{
			g_pD3DDevice->Release();
			g_pD3DDevice = NULL;
		}
		if( g_pD3D )
		{
			g_pD3D->Release();
			g_pD3D = NULL;
		}
		DestroyWindow( g_hWnd );
		g_hWnd = NULL;

		return false;
	}

	return true;
}

void InitDevice()
{
	//设置渲染状态
	//1 不使用灯光
	//2 使用顺时针拣选模式
	//3 不使用镜面反射
	//4 使用抖动
	g_pD3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	g_pD3DDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE);
	g_pD3DDevice->SetRenderState(D3DRS_DITHERENABLE, TRUE);

	//设置纹理选项
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0 );
	g_pD3DDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE );
	g_pD3DDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE );
	g_pD3DDevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE );
	g_pD3DDevice->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
	g_pD3DDevice->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	g_pD3DDevice->SetTextureStageState(0,D3DTSS_MIPFILTER,D3DTEXF_LINEAR);

	//设置world-view矩阵，使用正交投影
	D3DXMATRIX matView;
	D3DXMatrixIdentity( &matView );
	D3DXMatrixTranslation( &matView, 0, 0, 100 );
	g_pD3DDevice->SetTransform( D3DTS_VIEW, &matView );

	//设置view-projection矩阵
	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.33f, 0.1f, 10000.0f );
	g_pD3DDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	//设置世界矩阵
	D3DXMatrixIdentity(&g_matWorld);
	D3DXMatrixRotationY(&g_matWorld, D3DX_PI/2.0f );
	g_pD3DDevice->SetTransform( D3DTS_WORLD, &g_matWorld );

	//g_pD3DDevice->SetTransform( D3DTS_WORLD, &g_matWorld );
	
}

/**
 * @name  ObjectRotateAroundYAxis
 * @brief     物体绕Y轴旋转,即是改变model-world矩阵
 * @return    void 
 * @remark    
 **/
void ObjectRotateAroundYAxis()
{
	static float rotate_angle = 0.0f;
	rotate_angle += D3DX_PI/90.0f;
	D3DXMatrixRotationY(&g_matWorld, rotate_angle);
	g_pD3DDevice->SetTransform( D3DTS_WORLD, &g_matWorld );
}

void Render()
{
	if( !g_pD3DDevice )
		return;

	g_pD3DDevice->Clear( 0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET, 
		D3DCOLOR_XRGB(0,255,0), 1.0f, 0 );
    
	// pocz箃ek sceny
	g_pD3DDevice->BeginScene();

	g_pD3DDevice->SetTexture (0,g_pMD2Texture );

	if( g_MD2Model.Render( g_nFrame )) 
		g_nFrame = 0;

	g_pD3DDevice->EndScene();
	g_pD3DDevice->Present( NULL, NULL, NULL, NULL );

}
/**
 * @name  Run
 * @brief     
 * @return    void 
 * @remark    
 **/
   void Run()
{
	MSG msg;

	while(!GetAsyncKeyState(VK_ESCAPE))
	{
		while(GetAsyncKeyState(VK_SPACE))
			;
		
		if(GetAsyncKeyState(VK_LBUTTON))
		{ 
			g_nFrame++;
			//while(GetAsyncKeyState(VK_LBUTTON));
		}

		
		ObjectRotateAroundYAxis();
		Render();
		
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE  ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}
}

void CleanUp()
{
	if( g_pMD2Texture )
	{
		g_pMD2Texture->Release();
		g_pMD2Texture = NULL;
	}
	
	if( g_pD3DDevice )
	{
		g_pD3DDevice->Release();
		g_pD3DDevice = NULL;
	}

	if( g_pD3D )
	{
		g_pD3D->Release();
		g_pD3D = NULL;
	}
}


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	if( !InitWindow( hInstance ) )
	{
		MessageBox(g_hWnd,"无法创建窗口",
				   DEMOMSGBOXCAPTION , MB_OK|MB_ICONERROR);

	}

	if ( !InitD3D() )
	{
		MessageBox(g_hWnd,"无法初始化Direct3D",
				   DEMOMSGBOXCAPTION , MB_OK|MB_ICONERROR);
		return 0;
	};

	if( !InitTextures() )
	{
		MessageBox(g_hWnd,"无法创建纹理",
				   DEMOMSGBOXCAPTION , MB_OK|MB_ICONERROR);
		return 0;
	}

	if( !InitMD2Model() )
	{
		MessageBox(g_hWnd,"无法创建MD2模型" ,
				   DEMOMSGBOXCAPTION , MB_OK|MB_ICONERROR);
		return 0;
	}

	InitDevice();
	Run();
	CleanUp();
	
	return 0;
}

