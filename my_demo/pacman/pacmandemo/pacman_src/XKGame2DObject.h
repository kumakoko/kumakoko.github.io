/************************************************************************
 *	@file		：	PacManDemo/XKGame2DObject.h
 *	@brief		：	定义了2D的游戏object
 *
 *	@author		：	Xiong Xinke
 *	@copyright	：	Xiong Xinke
 *	@date		：	2005-2-22 10:59:33
 ************************************************************************/
#ifndef __PACMANDEMO_XKGAME2DOBJECT_H__
#define __PACMANDEMO_XKGAME2DOBJECT_H__

#include <d3dx8math.h>

#pragma warning(disable : 4786)

enum GAME_OBJ_TYPE
{
	OT_PLAYER,
	OT_GHOST
};

class XKGame2DObject  
{
public:
	XKGame2DObject();
	virtual ~XKGame2DObject();

public:
	void SetTranslation(const D3DXVECTOR2& translation );
	D3DXVECTOR2 GetTranslation() const;
	virtual void Update();
	virtual void Render2D();
	virtual void Collision( XKGame2DObject* p2DObj );
	bool	CheckIntersection( const XKGame2DObject* p2DObj );
	float	CheckCircleIntersection( const XKGame2DObject* p2DObj );
	D3DXVECTOR2	 GetLocation() const;
	D3DXVECTOR2	 GetCenter() const;
	void	SetLocation( const D3DXVECTOR2& vLoc );
	inline float GetWidth() const;
	inline void SetWidth( const float width );
	inline void SetHeight( const float height );
	inline float GetHeight() const;
	inline void SetBoundingRadius( const float& radius );
	inline float GetBoundingRadius() const;
	D3DXVECTOR2 GetCenterLocation() const;
	void SetCenterLocation(const D3DXVECTOR2& vLoc );
	GAME_OBJ_TYPE GetObjectType();
protected:
	D3DXVECTOR2	m_vLocation;		//该game object的二维坐标
	D3DXVECTOR2	m_RotationCenter;	//该object的旋转中心点
	float		m_fRotation;		//该object绕旋转中心点顺时针旋转的角度
	float		m_fWidth;
	float		m_fHeight;
	float		m_fBoundingRadius;
	GAME_OBJ_TYPE	m_ObjectType;
};

#endif /* __PACMANDEMO_XKGAME2DOBJECT_H__ */
