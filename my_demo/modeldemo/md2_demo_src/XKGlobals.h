  /************************************************************************
 *	@file		：	ModelDemo/XKGlobals.h
 *	@brief		：	
 *
 *	@author		：	Xiong Xinke
 *	@copyright	：	Xiong Xinke
 *	@date		：	2005-2-26 21:30:09
 ************************************************************************/
#ifndef __MODELDEMO_XKGLOBALS_H__
#define __MODELDEMO_XKGLOBALS_H__

#include <windows.h>
#include <tchar.h>
#include <d3dx8.h>

extern HWND					g_hWnd;
extern LPDIRECT3D8			g_pD3D;
extern LPDIRECT3DDEVICE8	g_pD3DDevice;
extern LPDIRECT3DTEXTURE8	g_pMD2Texture;

#define MD2_MAX_FRAMES		512		//一个MD2模型中最大的动作帧数
#define MD2_MAX_TRIANGLES	8192	//一个MD2模型中最大的三角形面片数
#define MD2_MAX_SKINS		32
#define MD2_MAX_SKINNAME	64
#define MD2_MAX_VERTS		4096
#define MD2_ANIMATION_SPEED 5.0f	//动画的播放速度

//Message Box的标题
#define DEMOMSGBOXCAPTION	(_T("ModelDemo"))
//md2文件所关联的skin纹理文件
#define MD2MODELTEXTURENAME (_T("md2tex.bmp"))
//md2文件名
#define MD2MODELFILE		(_T("md2file.md2"))

#define WINDOW_WIDTH		640
#define WINDOW_HEIGHT		480

#endif /* __MODELDEMO_XKGLOBALS_H__ */
