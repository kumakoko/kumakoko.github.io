/************************************************************************
 *	@file		：	PacManDemo/XKPacMaze.h
 *	@brief		：	定义了游戏世界——Maze
 *
 *	@author		：	Xiong Xinke
 *	@copyright	：	Xiong Xinke
 *	@date		：	2005-2-22 10:58:51
 ************************************************************************/
#ifndef __PACMANDEMO_XKPACMAZE_H__
#define __PACMANDEMO_XKPACMAZE_H__

#include <d3dx8.h>
#include <windows.h>
#include "XKSprite.h"

#pragma warning(disable : 4786)

//迷宫的TILE的类型

enum MAZE_TILE_TYPE
{
	TILE_BLOCK= 0x00,	//墙，不能通过的tile
	TILE_BLANK = 0x01, //路，能通过的tile
	TILE_ITEM = 0x02,  //物品，就是“豆子”
	TILE_POWERUP = 0x03	//力量，player吃了之后能够吃掉ghost
};

//player，ghost的移动方向
enum RUSH_DIR
{
	DIR_NONE ,
	DIR_UP,
	DIR_DOWN,
	DIR_LEFT,
	DIR_RIGHT
};

const POINT MAZE_SIZE = {20,15};// maze size, x and y
const int	TILE_SIZE = 40;		//每块瓷砖的大小，瓷砖一定是正方形							// block size

class XKPacMaze  
{
public:
	bool IsBlock( const POINT& Pos , RUSH_DIR dir );
	XKPacMaze();
	virtual ~XKPacMaze();

	D3DXVECTOR2	ToTranslation( const POINT& Pos );					// convert position (tile) to translation
	POINT		ToPosition( const D3DXVECTOR2& vTranslation );					// convert transltion to position
	POINT		ToAdjacentPosition( POINT Pos , RUSH_DIR dir );	// convert position to adjacent position
	D3DXVECTOR2 GetPlayerTranslation();						// returns hero's last translation (acquired from updateitems)
	POINT		GetPlayerPosition();							// returns hero's last position

	void		UpdateItems( const D3DXVECTOR2& vPlayerTranslation );			// eat items and what not.
	void		Render2D();									// render the maze
	bool		IsCenteredOnTile( const D3DXVECTOR2& vTranslation );		// returns true if the translation is centered on the tile
	bool		IsCenteredOnTileX( const D3DXVECTOR2& vTranslation );	// true if centered on x
	bool		IsCenteredOnTileY( const D3DXVECTOR2& vTranslation );	// true if centered on y

private:
	unsigned char*	m_pMap;			//pointer to the current map
	int				m_nNumItems;	//还剩下的豆子的数目，为0的时候表示过关
	XKSprite*		m_pTiles;
	D3DXVECTOR2		m_vPlayerTranslation ;//player的平移量
};

#endif /* __PACMANDEMO_XKPACMAZE_H__ */
