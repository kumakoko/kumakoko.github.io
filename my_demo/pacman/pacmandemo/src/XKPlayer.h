/************************************************************************
 *	@file		：	PacManDemo/XKPlayer.h
 *	@brief		：	定义了由玩家操纵的主角类
 *
 *	@author		：	Xiong Xinke
 *	@copyright	：	Xiong Xinke
 *	@date		：	2005-2-22 10:59:13
 ************************************************************************/
#ifndef __PACMANDEMO_XKPLAYER_H__
#define __PACMANDEMO_XKPLAYER_H__

#include "XKGame2DObject.h"
#include "XKPacMaze.h"


#pragma warning(disable : 4786)

class XKPlayer : public XKGame2DObject  
{
public:
	XKPlayer( XKPacMaze* pMaze );
	virtual ~XKPlayer();
	virtual void Update();
	virtual void Render2D();
	void ChangeDir( RUSH_DIR );
private:
	XKSprite*	m_pPlayer;
	XKPacMaze*	m_pPacMaze;
	RUSH_DIR	m_Dir;
	RUSH_DIR	m_LastDir;	//上一次的方向
	int			m_nFrameNumber;
	
};

#endif /* __PACMANDEMO_XKPLAYER_H__ */
