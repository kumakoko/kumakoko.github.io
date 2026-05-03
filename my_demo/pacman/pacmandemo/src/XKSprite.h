/************************************************************************
 *	@file		：	PacManDemo/XKSprite.h
 *	@brief		：	
 *
 *	@author		：	Xiong Xinke
 *	@copyright	：	Xiong Xinke
 *	@date		：	2005-2-22 16:20:29
 ************************************************************************/
#ifndef __PACMANDEMO_XKSPRITE_H__
#define __PACMANDEMO_XKSPRITE_H__

#include "XKTexture.h"
#include <d3dx8.h>

#pragma warning(disable : 4786)

class XKSprite  
{
public:
	XKSprite( ID3DXSprite* pSpr , XKTexture* pTex );
	XKSprite( ID3DXSprite* pSpr  , const string& FileName, 
			  D3DCOLOR ColorKey = NULL);
	virtual ~XKSprite();
	void SetSpriteTexture( XKTexture* pTex );
	void SetAnimationParams( int TotalAnim , int Rows , int Columns );
	
	void Render2D(const D3DXVECTOR2& translation = D3DXVECTOR2(0,0), 
				  const D3DXVECTOR2& rotCenter = D3DXVECTOR2(0,0), 
				  const float& rotation = 0,
				  const D3DXVECTOR2& scaling = D3DXVECTOR2(1,1),
				  D3DCOLOR color = D3DCOLOR_ARGB(255,255,255,255));
	void SetAnimation(int frame);
	XKTexture* GetXKTexture();
private:
	void Init();
	void InitAnimation();
	void UpdateRect();
	
protected:
	ID3DXSprite* m_pSprite;
	XKTexture*	 m_pTexture;		//sprite的纹理
	int          m_nWidth;			//sprite的高度
	int          m_nHeight;			//sprite的高度
	int          m_nRows;			//纹理中sprite帧的行数
	int          m_nColumns;		//纹理中sprite帧的列数
	int          m_nTotalAnim;		//纹理中动画的个数
	int          m_nCurrentAnim;	//当前要渲染的动画帧数
	D3DCOLOR	 m_ColorKey;		//透明色
	RECT         m_Rect;			//sprite的包络矩形
	bool		 m_bCreateTexture;	//当sprite的纹理创建成功时候设为true
	static D3DCOLOR m_sDefaultColorKey;
};

#endif /* __PACMANDEMO_XKSPRITE_H__ */
