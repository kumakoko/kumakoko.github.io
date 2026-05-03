/************************************************************************
 *	@file		：	PacManDemo/XKMath.h
 *	@brief		：	提供一系列和数学运算相关的接口
 *
 *	@author		：	Xiong Xinke
 *	@copyright	：	Xiong Xinke
 *	@date		：	2005-2-22 12:57:24
 ************************************************************************/
#ifndef __PACMANDEMO_XKMATH_H__
#define __PACMANDEMO_XKMATH_H__

#include <d3dx8math.h>
#include <math.h>

#pragma warning(disable : 4786)

/**
 * @name  XK_DistanceTo
 * @brief     计算两个二维矢量的距离
 * @return    float 
 * @param     const D3DXVECTOR2& vA
 * @param     const D3DXVECTOR2& vB
 * @remark    
 **/
float XK_DistanceTo( const D3DXVECTOR2& vA , const D3DXVECTOR2& vB )
{
	return sqrtf((vA.x - vB.x)*(vA.x - vB.x) +
				(vA.y - vB.y)*(vA.y - vB.y) );
}

/**
 * @name  XK_DistanceTo
 * @brief     计算两个三维矢量的距离
 * @return    float 
 * @param     const D3DXVECTOR3& vA
 * @param     const D3DXVECTOR3& vB
 * @remark    
 **/
float XK_DistanceTo( const D3DXVECTOR3& vA , const D3DXVECTOR3& vB )
{
	return sqrtf((vA.x - vB.x)*(vA.x - vB.x) +
				(vA.y - vB.y)*(vA.y - vB.y) +
				(vA.z - vB.z)*(vA.z - vB.z) );
}


#endif /* __PACMANDEMO_XKMATH_H__ */
