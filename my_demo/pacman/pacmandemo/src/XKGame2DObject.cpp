 /************************************************************************
 *	@file		：	PacManDemo/XKGame2DObject.cpp
 *	@brief		：	
 *
 *	@author		：	Xiong Xinke
 *	@copyright	：	Xiong Xinke
 *	@date		：	2005-2-22 11:00:33
 ************************************************************************/

#include "XKGame2DObject.h"
#include "XKMath.h"
#include "XKGlobals.h"

XKGame2DObject::XKGame2DObject()
{

}

XKGame2DObject::~XKGame2DObject()
{

}

/**
 * @name  XKGame2DObject::CheckCircleIntersection
 * @brief     检查两个2D game object的圆形包络盒是否相交
			  如果两个game object相交，计算出相交的两个圆
			  形包络盒的圆心的连线长度
 * @return    float 返回相交圆的圆心连线长度，当返回值为0或负数时表示不相交
 * @param     const XKGame2DObject* p2DObj
 * @remark    
 **/
float XKGame2DObject::CheckCircleIntersection( const XKGame2DObject* p2DObj )
{
	float actual_dist = XK_DistanceTo( GetCenterLocation(),
						p2DObj->GetCenterLocation() );
	float max_dist = GetBoundingRadius() +
					p2DObj->GetBoundingRadius();

	// > 0 相交 ； = 0 相切 ； < 0 相离
	return max_dist - actual_dist;
}

/**
 * @name  XKGame2DObject::CheckIntersection
 * @brief     检查两个2D game object的矩形包络盒是否相交
 * @return    bool 
 * @param     const XKGame2DObject* p2DObj
 * @remark    
 **/
bool XKGame2DObject::CheckIntersection( const XKGame2DObject* p2DObj )
{
	if( m_vLocation.x > p2DObj->GetLocation().x + p2DObj->GetWidth() ||
		p2DObj->GetLocation().x > m_vLocation.x + m_fWidth || 
		m_vLocation.y > p2DObj->GetLocation().y + p2DObj->GetHeight() ||
		p2DObj->GetLocation().y > m_vLocation.y + m_fHeight )
	{
		return false;
	}

	return true;
}

/**
 * @name  XKGame2DObject::Collision
 * @brief     碰撞检测，留给继承自XKGame2DObject的子类具体实现
 * @return    void 
 * @param     XKGame2DObject* p2DObj
 * @remark    
 **/
void XKGame2DObject::Collision( XKGame2DObject* p2DObj )
{

}

/**
 * @name  XKGame2DObject::Render2D
 * @brief     渲染接口，留给继承自XKGame2DObject的子类具体实现
 * @return    void 
 * @remark    
 **/
void XKGame2DObject::Render2D()
{

}

/**
 * @name  XKGame2DObject::Update
 * @brief     更新各种属性的接口，留给继承自
			  XKGame2DObject的子类具体实现
 * @return    void 
 * @remark    
 **/
void XKGame2DObject::Update()
{

}

/**
 * @name  XKGame2DObject::GetLocation
 * @brief     获取2D game object的二维空间坐标
 * @return    D3DXVECTOR2& 
 * @remark    
 **/
D3DXVECTOR2 XKGame2DObject::GetLocation() const
{
	return m_vLocation;
}

/**
 * @name  XKGame2DObject::GetCenter
 * @brief     获取该2DObject的半宽度和半长度
 * @return    D3DXVECTOR2 
 * @remark    
 **/
D3DXVECTOR2 XKGame2DObject::GetCenter() const
{
	return D3DXVECTOR2( m_fWidth/2.0f , m_fHeight/2.0f );
}

/**
 * @name  XKGame2DObject::SetLocation
 * @brief     设置2D game object的二维空间坐标
 * @return    void 
 * @param     const D3DXVECTOR2& vLoc
 * @remark    
 **/
void XKGame2DObject::SetLocation( const D3DXVECTOR2& vLoc )
{
	m_vLocation = vLoc;
}

/**
 * @name  XKGame2DObject::SetHeight
 * @brief     设置2D game object的矩形包络盒的高度
 * @return    void 
 * @param     const float height
 * @remark    
 **/
void XKGame2DObject::SetHeight( const float height )
{
	m_fHeight = height;
}

/**
 * @name  XKGame2DObject::GetHeight
 * @brief     获取2D game object的矩形包络盒的高度
 * @return    float
 * @remark    
 **/
float XKGame2DObject::GetHeight() const
{
	return m_fHeight;
}

/**
 * @name  XKGame2DObject::SetWidth
 * @brief     设置2D game object的矩形包络盒的宽度
 * @return    void 
 * @param     const float width
 * @remark    
 **/
void XKGame2DObject::SetWidth( const float width )
{
	m_fWidth = width;
}

/**
 * @name  XKGame2DObject::GetWidth
 * @brief     获取2D game object的矩形包络盒的宽度
 * @return    void 
 * @remark    
 **/
float XKGame2DObject::GetWidth() const
{
	return m_fWidth;
}

/**
 * @name  XKGame2DObject::SetBoundingRadius
 * @brief     设置该2D game object的圆形包络盒的半径
 * @return    void 
 * @param     const float& radius
 * @remark    
 **/
void XKGame2DObject::SetBoundingRadius( const float& radius )
{
	m_fBoundingRadius = radius;
}

/**
 * @name  XKGame2DObject::GetBoundingRadius
 * @brief     
 * @return    float 
 * @remark    
 **/
float XKGame2DObject::GetBoundingRadius() const
{
	return m_fBoundingRadius;
}

/**
 * @name  XKGame2DObject::GetCenterLocation
 * @brief     算出包络盒的中心点
 * @return    D3DXVECTOR2 
 * @remark    
 **/
D3DXVECTOR2 XKGame2DObject::GetCenterLocation() const
{
	return m_vLocation + GetCenter();
}

/**
 * @name  XKGame2DObject::SetCenterLocation
 * @brief     
 * @return    void 
 * @param     const D3DXVECTOR2& vLoc
 * @remark    
 **/
void XKGame2DObject::SetCenterLocation( const D3DXVECTOR2& vLoc )
{
	m_vLocation = vLoc - GetCenter();
}


/**
 * @name  XKGame2DObject::GetTranslation
 * @brief     
 * @return    D3DXVECTOR2 
 * @remark    
 **/
D3DXVECTOR2 XKGame2DObject::GetTranslation() const
{
	D3DXVECTOR2	translation;

	translation.x = m_vLocation.x ;//- TRANSLATION_ORIGIN_X;
	translation.y = m_vLocation.y ;//+ m_fHeight - TRANSLATION_ORIGIN_Y;
	
	return translation;
}

/**
 * @name  XKGame2DObject::SetTranslation
 * @brief     
 * @return    void 
 * @param     const D3DXVECTOR2& translation
 * @remark    
 **/
void XKGame2DObject::SetTranslation(const D3DXVECTOR2& translation )
{
	m_vLocation.x = translation.x ;//+ TRANSLATION_ORIGIN_X;
	m_vLocation.y = translation.y;// -TRANSLATION_ORIGIN_Y-m_fHeight;
}

/**
 * @name  XKGame2DObject::GetObjectType
 * @brief     
 * @return    void 
 * @remark    
 **/
GAME_OBJ_TYPE XKGame2DObject::GetObjectType()
{
	return m_ObjectType;
}