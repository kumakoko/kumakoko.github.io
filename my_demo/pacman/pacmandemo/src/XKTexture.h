/************************************************************************
 *	@file		：	PacManDemo/XKTexture.h
 *	@brief		：	
 *
 *	@author		：	Xiong Xinke
 *	@copyright	：	Xiong Xinke
 *	@date		：	2005-2-22 13:33:50
 ************************************************************************/
#ifndef __PACMANDEMO_XKTEXTURE_H__
#define __PACMANDEMO_XKTEXTURE_H__

#include <windows.h>
#include <d3d8.h>
#include <d3dx8.h>
#include <string>
#include <map>


#pragma warning(disable :4786)

using namespace std;

typedef struct 
{
	IDirect3DTexture8*  pTexture;		//纹理接口指针
	D3DXIMAGE_INFO		ImageInfo;		//该纹理的图像信息
	D3DCOLOR			ColorKey;		//缺省的透明色
	unsigned			nCount;			//该纹理的使用个数
	bool				bMultiLevel;	//
}XKTexInfo;

typedef map<string,XKTexInfo> TexInfoMap;

class XKTexture  
{
public:
	XKTexture( LPDIRECT3DDEVICE8 pDevice );
	virtual ~XKTexture();
	void CreateTexture( const string& savFile , int width, int height );
	void CreateTexture( const string& FileName , bool bMultiLevel, D3DCOLOR ColorKey);
	int GetWidth();
	int GetHeight();
	void BeginDrawing();
	void EndDrawing();
	string GetFileName();
	LPDIRECT3DTEXTURE8 GetTexture();
private:
	bool				m_bMultiLevel;
	int					m_nWidth;
	int					m_nHeight;
	D3DLOCKED_RECT		m_LockRect;
	LPDIRECT3DTEXTURE8	m_pTexture;
	LPDIRECT3DDEVICE8	m_pDevice;
	string				m_strFileName;
	unsigned char*		m_pPixels;		//指向纹理缓冲区的指针
	DWORD				m_dwPitch;
	static TexInfoMap	m_sInfoMap;
};

#endif /* __PACMANDEMO_XKTEXTURE_H__ */
