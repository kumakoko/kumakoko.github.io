/************************************************************************
 *	@file		：	PacManDemo/XKPlayer.cpp
 *	@brief		：	
 *
 *	@author		：	Xiong Xinke
 *	@copyright	：	Xiong Xinke
 *	@date		：	2005-2-22 11:01:21
 ************************************************************************/

#include "XKPlayer.h"
#include "XKGlobals.h"
#include <tchar.h>

XKPlayer::XKPlayer( XKPacMaze* pMaze )
{
	m_pPacMaze = pMaze;

	m_pPlayer = new XKSprite(g_pGame->GetD3DXSprite()
					,_T("pac-6-1-6.png"));
	
	m_nFrameNumber = 0;
	m_fRotation = 0.0f;
	
	m_fWidth = m_pPlayer->GetXKTexture()->GetWidth();
	m_fHeight = m_pPlayer->GetXKTexture()->GetHeight();
	m_RotationCenter.x = m_fWidth / 2;
	m_RotationCenter.y = m_fHeight / 2;

	POINT loc;
	loc.x = 10;
	loc.y = 9;

	SetTranslation( m_pPacMaze->ToTranslation(loc));

	m_Dir = DIR_NONE;
	m_LastDir = DIR_NONE;
}

XKPlayer::~XKPlayer()
{
	if( m_pPlayer )
	{
		delete m_pPlayer;
		m_pPlayer = NULL;
	}
}

/**
 * @name  XKPlayer::Update
 * @brief     
 * @return    void 
 * @remark    
 **/
void XKPlayer::Update()
{
	RUSH_DIR new_dir = DIR_NONE;

	if( GetAsyncKeyState(VK_UP) )
	{
		new_dir = DIR_UP;
	}

	if(GetAsyncKeyState(VK_DOWN))
	{
		new_dir = DIR_DOWN;
	}
	
	if(GetAsyncKeyState(VK_LEFT))
	{
		new_dir = DIR_LEFT;
	}
	
	if(GetAsyncKeyState(VK_RIGHT))
	{
		new_dir = DIR_RIGHT;
	}
	
	
	//获取player的像素坐标。并且把它转换成地图的TILE编号。
	D3DXVECTOR2 translation = GetTranslation();
	POINT current = m_pPacMaze->ToPosition(translation);

	//当player没有完全和一个TILE的大小重合的时候，
	//无论何种输入都不改变它的运动方向输入
	if( new_dir != DIR_NONE )
	{
		if( !m_pPacMaze->IsCenteredOnTile(translation))
		{
			switch( new_dir )
			{
			case DIR_DOWN:
			case DIR_UP:
				{
					if( m_Dir == DIR_LEFT ||
						m_Dir == DIR_RIGHT )
					new_dir = m_Dir;
				}
				break;
			case DIR_LEFT:
			case DIR_RIGHT:
				{
					if( m_Dir == DIR_UP ||
						m_Dir == DIR_DOWN )
					new_dir = m_Dir;
				}
				break;
			default:
				new_dir = m_Dir;
			}
		}
		else
		{
			if( m_pPacMaze->IsBlock( current , 
				new_dir ))
				new_dir = m_Dir;
		}
	}
	else
	{
		new_dir = m_Dir;
	}

	if( m_pPacMaze->IsCenteredOnTile(translation) &&
		m_pPacMaze->IsBlock( current , new_dir ) )
	{
		new_dir = DIR_NONE;
	}

	m_Dir = new_dir;

	if( g_pGame->GetGameTimer()->TimeMod(0.02f) == 0 )
	{
		switch( m_Dir )
		{
		case DIR_DOWN:
			m_vLocation.y += PLAYER_SPEED;
			m_fRotation = 0.0f;//3 * D3DX_PI/2;
			break;
		case DIR_UP:
			m_vLocation.y -= PLAYER_SPEED;
			m_fRotation = 0.0f;
			break;
		case DIR_LEFT:
			m_vLocation.x -= PLAYER_SPEED;
			m_fRotation = 0.0f;//D3DX_PI;
			break;
		case DIR_RIGHT:
			m_vLocation.x += PLAYER_SPEED;
			m_fRotation = 0.0f;
			break;
		default:
			break;
		}
	}
}
/**
 * @name  XKPlayer::Render2D
 * @brief     渲染主角类
 * @return    void 
 * @remark    
 **/
void XKPlayer::Render2D()
{
	if( g_pGame->GetGameTimer()
		->TimeMod(0.05f) == 0 )
	{
		//循环切换各帧
		++m_nFrameNumber %= 6;
		m_pPlayer->SetAnimation( m_nFrameNumber );
	}

	//根据player所旋转了的角度绘制player
	m_pPlayer->Render2D( GetTranslation(),
		m_RotationCenter,m_fRotation );
}

/**
 * @name  XKPlayer::ChangeDir
 * @brief     
 * @return    void 
 * @param     RUSH_DIR dir
 * @remark    
 **/
void XKPlayer::ChangeDir( RUSH_DIR dir )
{
//	m_Dir = dir;
}
