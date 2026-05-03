/************************************************************************
 *	@file		：	PacManDemo/XKGame.h
 *	@brief		：	定义了表示pac man游戏框架的类
 *
 *	@author		：	Xiong Xinke
 *	@copyright	：	Xiong Xinke
 *	@date		：	2005-2-22 10:50:36
 ************************************************************************/
#ifndef __PACMANDEMO_XKGAME_H__
#define __PACMANDEMO_XKGAME_H__

#include <d3d8.h>
#include <vector>
#include <windows.h>
#include "XKGhost.h"
#include "XKPacMaze.h"
#include "XKTimer.h"
#include "XKPlayer.h"

using namespace std;

#pragma warning(disable :4786)

class XKGame  
{
public:
	XKGame();
	virtual ~XKGame();
	HWND GetSafeHWND();
	void SetHWND( HWND hWnd );
	void InitWindow( HINSTANCE hInst , int nCmdShow );
	int	 MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void Run();
	LPD3DXSPRITE GetD3DXSprite();
	LPDIRECT3DDEVICE8 GetD3DDevice();
	XKTimer* GetGameTimer();
private:
	void InitD3D();
	void InitPacMaze();
	void CleanUp();
private:
	LPDIRECT3D8			m_pD3D;
	LPDIRECT3DDEVICE8	m_pD3DDevice;
	LPD3DXSPRITE		m_pD3DXSprite;
	XKGhost*			m_pGhost;
	XKPacMaze*			m_pPacMaze;
	XKPlayer*			m_pPlayer;
	XKTimer*			m_pTimer;
	HWND				m_hGameWnd;
};

#endif /* __PACMANDEMO_XKGAME_H__ */
