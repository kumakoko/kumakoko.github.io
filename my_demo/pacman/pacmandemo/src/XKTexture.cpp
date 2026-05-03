/************************************************************************
 *	@file		：	PacManDemo/XKTexture.cpp
 *	@brief		：	
 *
 *	@author		：	Xiong Xinke
 *	@copyright	：	Xiong Xinke
 *	@date		：	2005-2-22 13:34:02
 ************************************************************************/
#include "XKTexture.h"


TexInfoMap XKTexture::m_sInfoMap;

/**
 * @name  XKTexture::XKTexture
 * @brief     
 * @return    
 * @param     LPDIRECT3DDEVICE8 pDevice
 * @remark    
 **/
XKTexture::XKTexture( LPDIRECT3DDEVICE8 pDevice )
{
	m_pDevice = pDevice;
}

/**
 * @name  XKTexture::~XKTexture
 * @brief     
 * @return    
 * @remark    
 **/
XKTexture::~XKTexture()
{
	--(m_sInfoMap[m_strFileName].nCount);
}

/**
 * @name  XKTexture::CreateTexture
 * @brief     根据指定的宽和高创建一个空白的纹理
 * @return    
 * @param     const string& savFile
 * @param     int width
 * @param     int height
 * @remark    
 **/
void XKTexture::CreateTexture( const string& savFile , int width, int height )
{
	m_bMultiLevel = false;
	
	//首先要检查该texture是否已经被load进来了
	if( m_sInfoMap.size() )
	{
		if( m_sInfoMap.find( savFile) != m_sInfoMap.end() )
		{
			XKTexInfo tmp = m_sInfoMap[savFile];

			//如果该纹理已经被载入，却又想用不同的高，宽或者是层数
			//来再次载入，就要抛出一个异常
			if( m_bMultiLevel != tmp.bMultiLevel ||
				m_nWidth != width || m_nHeight != height )
			{
				//抛出异常
			}

			//如果符合规则地载入，就对其引用记数加一
			m_pTexture = m_sInfoMap[savFile].pTexture;
			++(m_sInfoMap[savFile].nCount);
			return;
		}
	}
	
	//如果该texture没有被load进来就新建一个
	XKTexInfo	info;
	HRESULT hr = D3DXCreateTexture( m_pDevice , 
		height , width , 0, D3DX_DEFAULT, D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED, &(info.pTexture));
	if( FAILED(hr) )
	{
		//抛出异常
	}
	
	info.nCount = 1;
	info.bMultiLevel = m_bMultiLevel;
	info.ImageInfo.Height = height;
	info.ImageInfo.Width = width;
	info.ColorKey = NULL;
	
	m_pTexture = info.pTexture;
	m_sInfoMap[savFile] = info;
	m_pPixels = NULL;
	
}

/**
 * @name  XKTexture::CreateTexture
 * @brief    从文件中载入数据创建一个纹理
 * @return    
 * @param     const string& FileName
 * @param     bool bMultiLevel			是否使用mipmap
 * @param     D3DCOLOR ColorKey
 * @remark    
 **/
void XKTexture::CreateTexture( const string& FileName , bool bMultiLevel, D3DCOLOR ColorKey )
{
	m_bMultiLevel = bMultiLevel;
	m_strFileName = FileName;
	m_pTexture = NULL;

	//首先要检查该texture是否已经被load进来了
	if( m_sInfoMap.size() )
	{
		if( m_sInfoMap.find( FileName) != m_sInfoMap.end() )
		{
			XKTexInfo tmp = m_sInfoMap[FileName];

			//如果该纹理已经被载入，却又想用不同的高，宽或者是层数
			//来再次载入，就要抛出一个异常
			if( m_bMultiLevel != tmp.bMultiLevel ||
				m_sInfoMap[FileName].ColorKey != ColorKey )
			{
				//抛出异常
			}

			//如果符合规则地载入，就对其引用记数加一
			m_pTexture = m_sInfoMap[FileName].pTexture;
			++(m_sInfoMap[FileName].nCount);
			return;
		}
	}

	HRESULT		hr;
	XKTexInfo	info;

	//如果使用多级渐进纹理过滤（mipmap）
	if( bMultiLevel )
	{
		//函数的filter参数为D3DX_DEFAULT，等于D3DX_FILTER_TRIANGLE | D3DX_FILTER_DITHER
		hr = D3DXCreateTextureFromFileEx( m_pDevice, FileName.c_str(), 
				0, 0, D3DX_DEFAULT, 0, D3DFMT_A8R8G8B8, 
				D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
				ColorKey, &(info.ImageInfo), NULL, &(info.pTexture));
	}
	else
	{
		//函数的filter参数为D3DX_DEFAULT,不使用过滤
		hr = D3DXCreateTextureFromFileEx( m_pDevice, FileName.c_str(), 
				0, 0, D3DX_DEFAULT, 0, D3DFMT_A8R8G8B8, 
				D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_DEFAULT, 
				ColorKey, &(info.ImageInfo), NULL, &(info.pTexture));
	}

	if( FAILED(hr) )
	{
		//抛出异常
	}
	
	info.nCount = 1;
	info.bMultiLevel = m_bMultiLevel;
	m_nHeight =  info.ImageInfo.Height;
	m_nWidth = info.ImageInfo.Width;
	info.ColorKey = NULL;
	
	m_pTexture = info.pTexture;
	m_sInfoMap[FileName] = info;
	m_pPixels = NULL;
}

/**
 * @name  XKTexture::GetHeight
 * @brief     返回纹理的高度，以像素为单位
 * @return    int 
 * @remark    
 **/
int XKTexture::GetHeight()
{
	return m_sInfoMap[m_strFileName].ImageInfo.Height;
}

/**
 * @name  XKTexture::GetWidth
 * @brief     返回纹理的宽度，以像素为单位
 * @return    int 
 * @remark    
 **/
int XKTexture::GetWidth()
{
	return m_sInfoMap[m_strFileName].ImageInfo.Width;
}

/**
 * @name  XKTexture::BeginDrawing
 * @brief     在开始绘制纹理的时候的一些预先步骤
 * @return    void 
 * @remark    
 **/
void XKTexture::BeginDrawing()
{
	if( m_bMultiLevel )
	{
		//抛出异常
	}

	//锁定整个矩形
	ZeroMemory( &m_LockRect , sizeof(D3DLOCKED_RECT));
	HRESULT hr = m_pTexture->LockRect( 0 , 
				 &m_LockRect , NULL , 0 );
	
	if( FAILED(hr) )
	{
		//抛出异常
	}
	//根据锁定的矩形区域获取跨度和纹理缓冲区的首指针
	m_dwPitch = m_LockRect.Pitch; 
	m_pPixels = (unsigned char*)m_LockRect.pBits;
}

/**
 * @name  XKTexture::EndDrawing
 * @brief     在结束绘制纹理的时的一些收尾步骤
 * @return    void 
 * @remark    
 **/
void XKTexture::EndDrawing()
{
	if( m_bMultiLevel )
	{
		//抛出异常
	}

	m_pTexture->UnlockRect(0);
	m_dwPitch = 0;
	m_pPixels = NULL;
}

/**
 * @name  XKTexture::GetFileName
 * @brief     
 * @return    string 
 * @remark    
 **/
string XKTexture::GetFileName()
{
	return m_strFileName;
}

/**
 * @name  XKTexture::GetTexture
 * @brief     
 * @return    LPDIRECT3DTEXTURE8 
 * @remark    
 **/
LPDIRECT3DTEXTURE8 XKTexture::GetTexture()
{
	return m_pTexture;
}
