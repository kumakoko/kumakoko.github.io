/************************************************************************
 *	@file		：	PacManDemo/XKGhost.h
 *	@brief		：	定义了Player的敌人——ghost的类，ghost也是
					一个2D的游戏object
 *
 *	@author		：	Xiong Xinke
 *	@copyright	：	Xiong Xinke
 *	@date		：	2005-2-22 10:58:25
 ************************************************************************/
#ifndef __PACMANDEMO_XKGHOST_H__
#define __PACMANDEMO_XKGHOST_H__

#include "XKGame2DObject.h"
#include "XKPacMaze.h"

#pragma warning(disable : 4786)

//恶魔的状态
enum GHOST_STATE 
{ 
	GHOST_STRONG,	//强壮的状态，这时候player可以被恶魔杀害
	GHOST_FLASHING, //恶魔闪烁的状态
	GHOST_WEAK, 
	GHOST_DEAD		//恶魔死亡的状态
};

//恶魔的AI
enum GHOST_AI
{ 
	GHOST_AGGRESIVE, //追击player
	GHOST_PASSIVE,	 //
	GHOST_EVASIVE	 //逃避player
};

class XKGhost : public XKGame2DObject  
{
public:
	XKGhost( XKPacMaze* pMaze );
	virtual ~XKGhost();
	
	virtual void Update();
	virtual void Render2D();
	virtual void Collision(XKGame2DObject* p2DObj );
	void	MakeWeak();
private:

	RUSH_DIR RandomDirection();		//随机指定的方向

	GHOST_STATE m_State;			//ghost的当前状态
	GHOST_AI	m_AIState;			//ghost的AI策略状态
	XKSprite*	m_pGhost;			//ghost的sprite对象 
	int			m_nFrameNumber;		//ghost的sprite帧编号
	RUSH_DIR	m_Dir;				//ghost运动的方向
	float		m_fWeakStartTime;	//当ghost开始变得可被player攻击时的时刻
	XKPacMaze*	m_pPacMaze;			//指向地图的指针
};
#endif /* __PACMANDEMO_XKGHOST_H__ */
