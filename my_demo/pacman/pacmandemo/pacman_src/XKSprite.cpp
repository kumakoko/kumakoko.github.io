/************************************************************************
 *	@file		：	PacManDemo/XKSprite.cpp
 *	@brief		：	定义了2D精灵类
 *
 *	@author		：	Xiong Xinke
 *	@copyright	：	Xiong Xinke
 *	@date		：	2005-2-22 16:20:39
 ************************************************************************/
#include "XKSprite.h"
#include "XKMiscTools.h"
#include "XKGlobals.h"
#include "XKError.h"
#include <tchar.h>

#pragma  warning(disable : 4786)

D3DCOLOR XKSprite::m_sDefaultColorKey = NULL;

/**
 * @name  XKSprite::XKSprite
 * @brief     
 * @return    
 * @param     ID3DXSprite* pSpr
 * @param     XKTexture* pTex
 * @remark    
 **/
XKSprite::XKSprite( ID3DXSprite* pSpr , XKTexture* pTex )
{
	m_pSprite = pSpr;
	m_pTexture = pTex;	
	m_ColorKey = m_sDefaultColorKey;
	m_bCreateTexture = false;
	Init();
}

/**
 * @name  XKSprite::XKSprite
 * @brief     
 * @return    
 * @param      ID3DXSprite* pSpr
 * @param     const string& FileName
 * @param     D3DCOLOR ColorKey
 * @remark    
 **/
XKSprite::XKSprite( ID3DXSprite* pSpr , const string& FileName, D3DCOLOR ColorKey )
{
	m_pSprite = pSpr;
	m_pTexture = NULL;	
	m_pTexture = new XKTexture( g_pGame->GetD3DDevice() );
	m_pTexture->CreateTexture( FileName , false , ColorKey );

	if( ColorKey == NULL ) 
	{
		m_ColorKey = m_sDefaultColorKey;
	}
	else
	{
		m_ColorKey = ColorKey;	
	}

	m_bCreateTexture = true;
	Init();
}

/**
 * @name  XKSprite::~XKSprite
 * @brief     
 * @return    
 * @remark    
 **/
XKSprite::~XKSprite()
{

}


/**
 * @name  XKSprite::Init
 * @brief     初始化好sprite的初始帧数等
 * @return    void 
 * @remark    
 **/
void XKSprite::Init()
{
	m_nTotalAnim	 = 1;
	m_nRows			 = 1;
	m_nColumns		 = 1;
	
	InitAnimation();
}

/**
 * @name  XKSprite::InitAnimation
 * @brief     
 * @return    void 
 * @remark    
 **/
void XKSprite::InitAnimation()
{
	int num_pos , column_pos , row_pos;
	
	//缺省地，sprite不存在动画，所以先把一些数据设置为1
	int total_anim_tmp = 1;
	int row_tmp = 1;
	int column_tmp = 1;

	//在本demo中，图片文件的文件名就包含了其动画帧的相关信息
	//图片文件的命名约定为：
	
	//图片内容名-总动画帧数-列数-行数.后缀名.
	
	//例如有图形文件 bear-20-5-4.bmp,就表示：
	//该图片有20帧图素，分为5列，6行。是位图文件
	string TexFileName = m_pTexture->GetFileName();
	num_pos = TexFileName.find(_T("-")) + 1;
	column_pos = TexFileName.find(_T("-") , num_pos ) + 1;
	row_pos = TexFileName.find( _T("-") , column_pos ) + 1;
	
	//如果这三个数有一个小于等于0的话，表
	//示该sprite不存在动画就使用缺省的
	if( num_pos <= 0 || column_pos <= 0 || row_pos <= 0 )
	{

	}
	//否则根据文件名中的数字计算帧数
	else
	{
		total_anim_tmp = XK_StringToInt( 
			TexFileName.substr(num_pos,column_pos-1) );
		column_tmp = XK_StringToInt(
			TexFileName.substr(column_pos, row_pos - 1) );
		row_tmp = XK_StringToInt(
			TexFileName.substr( row_pos , 
			TexFileName.find(_T(".") ) ) );
	}

	SetAnimationParams( total_anim_tmp , row_tmp , column_tmp );
}

/**
 * @name  XKSprite::SetAnimationParams
 * @brief     
 * @return    void 
 * @param     int TotalAnim
 * @param     int Rows
 * @param     int Columns
 * @remark    
 **/
void XKSprite::SetAnimationParams( int TotalAnim , int Rows , int Columns )
{
	if( TotalAnim <= 0 || Rows <= 0 || Columns <= 0 )
		return;

	if( Rows * Columns < TotalAnim )
	{
		//抛出异常
	}

	m_nTotalAnim = TotalAnim;
	m_nColumns = Columns;
	m_nRows = Rows;
	
	//根据纹理的高宽和行列数，求出该sprite的宽高
	m_nWidth = m_pTexture->GetWidth() / m_nColumns;
	m_nHeight = m_pTexture->GetHeight() / m_nRows;
	
	m_nCurrentAnim = 0;
	UpdateRect();
}

/**
 * @name  XKSprite::UpdateRect
 * @brief     更新当前要播放的动画帧所在的矩形
 * @return    void 
 * @remark    
 **/
void XKSprite::UpdateRect()
{
	m_Rect.left = (m_nCurrentAnim % m_nColumns) * m_nWidth;
	m_Rect.top = (m_nCurrentAnim / m_nColumns) * m_nWidth;
	m_Rect.right = m_Rect.left + m_nWidth;
	m_Rect.bottom = m_Rect.top + m_nHeight;
}

/**
 * @name  XKSprite::Render2D
 * @brief     利用ID3DXSprite接口渲染2D sprite
 * @return    void 
 * @param     const D3DXVECTOR2& translation 渲染sprite的时候的平移量
 * @param     const D3DXVECTOR2& rotCenter 如果渲染sprite要旋转一定角度，那么这个参数就是指定sprite旋转的轴点
 * @param     const float& rotation 渲染sprite的时候所要旋转的角度，以弧度为单位，顺时针
 * @param     const D3DXVECTOR2& scaling 渲染sprite的时候所放缩的比例
 * @param     D3DCOLOR color 渲染sprite的颜色
 * @remark    
 **/
void XKSprite::Render2D(const D3DXVECTOR2& translation , const D3DXVECTOR2& rotCenter , const float& rotation ,const D3DXVECTOR2& scaling , D3DCOLOR color )
{
	if( m_pTexture == NULL )
	{
		//抛出异常
	}

	HRESULT hr = m_pSprite->Draw( m_pTexture->GetTexture() , 
		&m_Rect ,&scaling , &rotCenter , 
		rotation , &translation , color );

	if( FAILED(hr) )
	{
		throw XKError("无法渲染2D object",__FILE__,
			__LINE__ , hr );
	}
	
}

/**
 * @name  XKSprite::SetAnimation
 * @brief     
 * @return    void 
 * @param     int frame
 * @remark    
 **/
void XKSprite::SetAnimation( int frame )
{
	if( frame < 0 || frame >= m_nTotalAnim )
	{
		frame = 0;
	}

	m_nCurrentAnim = frame;
	UpdateRect();				// update the bounding rectangle
}

/**
 * @name  XKSprite::GetXKTexture
 * @brief     
 * @return    XKTexture* 
 * @remark    
 **/
XKTexture* XKSprite::GetXKTexture()
{
	return m_pTexture;
}


