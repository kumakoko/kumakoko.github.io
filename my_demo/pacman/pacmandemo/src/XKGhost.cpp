/************************************************************************
 *	@file		：	PacManDemo/XKGhost.cpp
 *	@brief		：	
 *
 *	@author		：	Xiong Xinke
 *	@copyright	：	Xiong Xinke
 *	@date		：	2005-2-22 11:00:46
 ************************************************************************/
#include "XKGhost.h"
#include "XKGlobals.h"
#include <stdlib.h>
#include <TCHAR.H>

XKGhost::XKGhost( XKPacMaze* pMaze )
{
	m_pPacMaze = pMaze;
	m_pGhost = new XKSprite(g_pGame->GetD3DXSprite()
		, _T("ghost-8-4-2.bmp"),D3DCOLOR_XRGB(0,0,0) );
	
	//设置各项属性
	m_fWidth	= m_pGhost->GetXKTexture()->GetWidth();
	m_fHeight	= m_pGhost->GetXKTexture()->GetHeight();
	m_ObjectType	= OT_GHOST;
	m_nFrameNumber	= 0;
	m_fBoundingRadius = 8.0f;
	m_fWeakStartTime = 0;
	m_State = GHOST_STRONG;
	m_AIState	= GHOST_PASSIVE;
	m_Dir	= RandomDirection();

	
	//设置ghost的诞生位置
	POINT loc = {7,9};
	SetTranslation( m_pPacMaze->ToTranslation(loc) );
}

XKGhost::~XKGhost()
{
	if( m_pGhost )
	{
		delete m_pGhost;
		m_pGhost = NULL;
	}
}


/**
 * @name  XKGhost::Update
 * @brief     更新恶魔的状态
 * @return    void 
 * @remark    
 **/
void XKGhost::Update() 
{
	//获取player的坐标位置
	D3DXVECTOR2 player_pos = m_pPacMaze->GetPlayerTranslation();
	//获取本Ghost的位置
	D3DXVECTOR2 my_pos = GetTranslation();
	//计算本ghost在地图中的编号位置
	POINT current = m_pPacMaze->ToPosition(my_pos);

	//每5秒就改变一次本ghost的AI状态
	if( g_pGame->GetGameTimer()->TimeMod(5) == 0)
	{
		m_AIState = (GHOST_AI)(rand() % 3);
	}

	//如果此时本ghost完全是只站在一个TILE中的时候
	if( m_pPacMaze->IsCenteredOnTile(my_pos) ) 
	{
		switch( m_State ) 
		{
		case GHOST_DEAD:
			break;
		case GHOST_STRONG://如果现在ghost可以吃掉player
						  //就根据AI策略进行行动
			switch( m_AIState )
			{
			case GHOST_AGGRESIVE:	
			case GHOST_EVASIVE:	
				//根据随机数来决定ghost是沿X
				//轴方向还是Y轴方向来追踪player
				if( (rand() % 2) == 0 )
				{//根据player的位置来判断究竟本ghost如何走 
					if( player_pos.x > my_pos.x)
					{
						m_Dir = DIR_RIGHT;
					} 
					else if( player_pos.x < my_pos.x)
					{
						m_Dir = DIR_LEFT; 
					} 
					else if( player_pos.y > my_pos.y)
					{ 
						m_Dir = DIR_DOWN; 
					} 
					else 
					{
						m_Dir = DIR_UP;
					}
					
				} 
				else
				{   
					//沿Y方向追踪
					if( player_pos.y > my_pos.y)
					{
						m_Dir = DIR_DOWN;
					}
					else if( player_pos.y < my_pos.y)
					{
						m_Dir = DIR_UP;
					}
					else if( player_pos.x > my_pos.x)
					{
						m_Dir = DIR_RIGHT; 
					} else {
						m_Dir = DIR_LEFT;
					}
				}	
				break;
			default:
				m_Dir = RandomDirection();
				break;
			}
			break;

			case GHOST_FLASHING:
			case GHOST_WEAK:

			switch (m_AIState)
			{
			case GHOST_AGGRESIVE:	
			case GHOST_EVASIVE:	//根据AI策略决定ghost如何逃避player
				if( rand() % 2 == 0) 
				{ 
					//Run awayin X axis
					if( player_pos.x > my_pos.x)
					{
						m_Dir = DIR_LEFT;
					} 
					else
					{
						m_Dir = DIR_RIGHT; 
					}
					
				} 
				else
				{
					if( player_pos.y > my_pos.y) 
					{
						m_Dir = DIR_UP;
					}
					else
					{
						m_Dir = DIR_DOWN; 
					}
				}	
				break;
			default:
				m_Dir = RandomDirection();
				break;
			}
		}

		//确保ghost不能冲进阻碍物里面去
		while( m_pPacMaze->IsBlock( current, m_Dir) )
		{
			m_Dir = RandomDirection();
		}
	}

	//如果ghost没有死亡的话就继续移动
	if( m_State != GHOST_DEAD)
	{
		if( g_pGame->GetGameTimer()->TimeMod(0.02f) == 0) 
		{
			switch(m_Dir)
			{
			case DIR_DOWN:
				m_vLocation.y -= GHOST_SPEED;
				break;
			case DIR_UP:
				m_vLocation.y += GHOST_SPEED;
				break;
			case DIR_LEFT:
				m_vLocation.x -= GHOST_SPEED;
				break;
			case DIR_RIGHT:
			default:
				m_vLocation.x += GHOST_SPEED;
				break;
			}
		}
	}
}

/**
 * @name  XKGhost::Render2D
 * @brief     
 * @return    void 
 * @remark    
 **/
void XKGhost::Render2D()
{
	//每50毫秒切换一帧
	if( g_pGame->GetGameTimer()->TimeMod(0.05f) == 0)
	{
		++m_nFrameNumber %= 4;
	}
	
	m_pGhost->SetAnimation( m_nFrameNumber );

	m_pGhost->Render2D(GetTranslation(), D3DXVECTOR2(0,0),
		0, D3DXVECTOR2(1,1), D3DCOLOR_ARGB( 255, 255, 255, 255));
}


/**
 * @name  XKGhost::Collision
 * @brief     碰撞检测
 * @return    void 
 * @param     XKGame2DObject* p2DObj
 * @remark    
 **/
void XKGhost::Collision(XKGame2DObject* p2DObj )
{

}

/**
 * @name  XKGhost::RandomDirection
 * @brief     
 * @return    RUSH_DIR 
 * @remark    
 **/
RUSH_DIR XKGhost::RandomDirection()
{
	switch( rand() % 4) 
	{
	case 0:
		return DIR_DOWN;
	case 1:
		return DIR_UP;
	case 2: 
		return DIR_LEFT;
	case 3:
	default:
		return DIR_RIGHT;
	}
}

