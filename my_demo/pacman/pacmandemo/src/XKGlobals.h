 /************************************************************************
 *	@file		：	PacManDemo/XKGlobals.h
 *	@brief		：	定义了一系列的全局变量
 *
 *	@author		：	Xiong Xinke
 *	@copyright	：	Xiong Xinke
 *	@date		：	2005-2-22 22:11:36
 ************************************************************************/
#ifndef __PACMANDEMO_XKGLOBALS_H__
#define __PACMANDEMO_XKGLOBALS_H__

#include "XKGame.h"
#include <windows.h>

#pragma warning(disable : 4786)

extern XKGame*		g_pGame;
extern HWND			g_hGameWnd;
extern HINSTANCE	g_hGameInst;
extern bool			g_bWindowed;

const float TRANSLATION_ORIGIN_X = 0.0f;
const float TRANSLATION_ORIGIN_Y = 600.0f;
const float PLAYER_SPEED = 8.0f;
const float GHOST_SPEED = 4.0f;
#endif /* __PACMANDEMO_XKGLOBALS_H__ */
