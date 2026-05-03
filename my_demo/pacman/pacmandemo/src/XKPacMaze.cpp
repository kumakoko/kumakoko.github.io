/************************************************************************
 *	@file		：	PacManDemo/XKPacMaze.cpp
 *	@brief		：	
 *
 *	@author		：	Xiong Xinke
 *	@copyright	：	Xiong Xinke
 *	@date		：	2005-2-22 11:01:04
 ************************************************************************/
#include "XKPacMaze.h"
#include "XKError.h"
#include "XKGlobals.h"
#include <tchar.h>
#include <WINDOWS.H>
#include <fstream>

using namespace std;

XKPacMaze::XKPacMaze()
{
	m_pMap = new unsigned char[ MAZE_SIZE.x * MAZE_SIZE.y ];

	if( m_pMap == NULL )
	{
		throw XKError("无法申请内存空间，无法创建地图",__FILE__,__LINE__);
	}

	//创建tile sprite
	m_pTiles = new XKSprite( g_pGame->GetD3DXSprite()
					,_T("pac_tiles-4-4-1.png") );
	
	//读取地图数据文件
	string MapdataLine;
	MapdataLine.erase( MapdataLine.begin() , 
					   MapdataLine.end() );
	
	ifstream MapFile;
	MapFile.open("mapdata.txt");


	for( int row = 0 ; row < MAZE_SIZE.y ; ++row )
	{
		getline(MapFile,MapdataLine);
		string::iterator it = MapdataLine.begin();	
		
		for( int column = 0 ; column < MAZE_SIZE.x ;
			 ++column )
		{
			switch( *it )
			{
			case '0':
				m_pMap[row * MAZE_SIZE.x + column] = TILE_BLOCK;
				break;
			case '2':
				m_pMap[row * MAZE_SIZE.x + column] = TILE_ITEM;
				break;
			case '3':
				m_pMap[row * MAZE_SIZE.x + column] = TILE_POWERUP;
				break;
			}

			++it;
		}
		MapdataLine.erase(MapdataLine.begin() , MapdataLine.end() );	 
	}

	MapFile.close();
}

/**
 * @name  XKPacMaze::~XKPacMaze
 * @brief     释放所有申请的资源
 * @return    
 * @remark    
 **/
XKPacMaze::~XKPacMaze()
{
	if( m_pMap )
	{
		delete[] m_pMap;
		m_pMap = NULL;
	}

	if( m_pTiles )
	{
		delete m_pTiles;
		m_pTiles = NULL;
	}
}

/**
 * @name  XKPacMaze::GetPlayerPosition
 * @brief     获取player的所在瓷砖的编号
 * @return    D3DXVECTOR2 
 * @remark    
 **/
POINT XKPacMaze::GetPlayerPosition()
{
	return ToPosition(m_vPlayerTranslation);
}

/**
 * @name  XKPacMaze::GetPlayerTranslation
 * @brief	  
 * @return    D3DXVECTOR2 
 * @remark    
 **/
D3DXVECTOR2 XKPacMaze::GetPlayerTranslation()
{
	return m_vPlayerTranslation;
}

/**
 * @name  XKPacMaze::IsBlock
 * @brief     检查要移动去的tile是否是block
 * @return    bool 
 * @param     const POINT& Pos
 * @param     RUSH_DIR dir
 * @remark    
 **/
bool XKPacMaze::IsBlock( const POINT& Pos , RUSH_DIR dir )
{
	POINT position = ToAdjacentPosition( Pos, dir );

	if( m_pMap[position.y * MAZE_SIZE.x + position.x] 
		== TILE_BLOCK)
	{
		return true;
	}
	
	return false;
}

/**
 * @name  XKPacMaze::IsCenteredOnTile
 * @brief     
 * @return    bool 
 * @param     D3DXVECTOR2 vTranslation
 * @remark    
 **/
bool XKPacMaze::IsCenteredOnTile( const D3DXVECTOR2& vTranslation )
{
	if( (int)vTranslation.x % TILE_SIZE == 0 && 
		(int)vTranslation.y % TILE_SIZE == 0 )
		return true;
	
	return false;
}

/**
 * @name  XKPacMaze::IsCenteredOnTileX
 * @brief     
 * @return    bool 
 * @param     const D3DXVECTOR2& vTranslation
 * @remark    
 **/
bool XKPacMaze::IsCenteredOnTileX( const D3DXVECTOR2& vTranslation )
{
	if( (int)vTranslation.x % TILE_SIZE == 0 )
		return true;
	return false;
}

/**
 * @name  XKPacMaze::IsCenteredOnTileY
 * @brief     判断某object是否在TILE的中间
 * @return    bool 
 * @param     const D3DXVECTOR2& vTranslation
 * @remark    
 **/
bool XKPacMaze::IsCenteredOnTileY( const D3DXVECTOR2& vTranslation )
{
	if( (int)vTranslation.y % TILE_SIZE == 0 )
	  return true;

	return false;
}

/**
 * @name  XKPacMaze::Render2D
 * @brief     绘制地图
 * @return    void 
 * @remark    
 **/
void XKPacMaze::Render2D()
{
	for( int i = 0 ; i < MAZE_SIZE.y ; i++ )
	{
		for( int j = 0 ; j < MAZE_SIZE.x ; j++ )
		{
			//如果某TILE是powerup类型的话，每隔一段时间就要让TILE
			//变成BLANK类型，以便产生powerup闪烁效果。
			
			if( m_pMap[i*MAZE_SIZE.x+j] == TILE_POWERUP
				&& g_pGame->GetGameTimer()->TimeMod(1) <= 0.3f )
			{
				//切换到BLANK TILE的那一帧
				m_pTiles->SetAnimation( TILE_BLANK );
			}
			else
			{
				m_pTiles->SetAnimation( 
					m_pMap[i*MAZE_SIZE.x+j] );
			}

			//m_pTiles->SetAnimation( m_pMap[i*MAZE_SIZE.x+j] );
			POINT translation;
			translation.x = j;
			translation.y = i;
			
			m_pTiles->Render2D( ToTranslation(translation) );
		}
	}
	//m_pTiles->Render2D(D3DXVECTOR2(0.0f,0.0f));
}

/**
 * @name  XKPacMaze::ToAdjacentPosition
 * @brief     根据传递进来的移动方向调整object的所在的瓷砖的编号
 * @return    POINT 
 * @param     POINT			vPos
 * @param     RUSH_DIR		dir
 * @remark    
 **/
POINT XKPacMaze::ToAdjacentPosition( POINT Pos , RUSH_DIR dir )
{
	switch( dir )
	{
	case DIR_UP:
		Pos.y -= 1;
		break;
	case DIR_DOWN:
		Pos.y += 1;
		break;
	case DIR_LEFT:
		Pos.x -= 1;
		break;
	case DIR_RIGHT:
		Pos.x += 1;
		break;
	case DIR_NONE:
		break;
	}

	return Pos;
}


/**
 * @name  XKPacMaze::ToPosition
 * @brief     把实际的坐标(translation)转换为
			  瓷砖的编号(position)
 * @return    POINT 
 * @param     D3DXVECTOR2 vTranslation 实际的坐标
 * @remark    
 **/
POINT XKPacMaze::ToPosition( const D3DXVECTOR2& vTranslation )
{
	POINT rt;
	rt.x = (int)(vTranslation.x / TILE_SIZE );
	rt.y = (int)(vTranslation.y / TILE_SIZE );

	return rt;
}


/**
 * @name  XKPacMaze::ToTranslation
 * @brief     把瓷砖的编号(position)转换为
			  实际的坐标(translation)
 * @return    D3DXVECTOR2 
 * @param     POINT vPos
 * @remark    
 **/
D3DXVECTOR2 XKPacMaze::ToTranslation( const POINT& Pos )
{
	return D3DXVECTOR2( Pos.x*TILE_SIZE , Pos.y*TILE_SIZE );
}

/**
 * @name  XKPacMaze::UpdateItems
 * @brief     player吃掉了豆子后，要更新那个位置的tile的显示
 * @return    void 
 * @param     D3DXVECTOR2 vPlayerTranslation
 * @remark    
 **/
void XKPacMaze::UpdateItems( const D3DXVECTOR2& vPlayerTranslation )
{
	m_vPlayerTranslation = vPlayerTranslation;

	POINT PlayerPos = ToPosition( vPlayerTranslation );

	if( IsCenteredOnTile( vPlayerTranslation) )
	{
		int index = PlayerPos.y * MAZE_SIZE.x + PlayerPos.x;
		
		switch( m_pMap[index] )
		{
		case TILE_ITEM:
			m_pMap[index] = TILE_BLANK;
			--m_nNumItems;
			break;
		case TILE_POWERUP:
			m_pMap[index] = TILE_BLANK;
			--m_nNumItems;
			break;
		}
	}
}
