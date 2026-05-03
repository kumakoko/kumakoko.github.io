/************************************************************************
 *	@file		：	PacManDemo/XKMiscTools.h
 *	@brief		：	一些相关的杂项工具函数
 *
 *	@author		：	Xiong Xinke
 *	@copyright	：	Xiong Xinke
 *	@date		：	2005-2-22 17:31:10
 ************************************************************************/
#ifndef __PACMANDEMO_XKMISCTOOLS_H__
#define __PACMANDEMO_XKMISCTOOLS_H__

#include <string>

using namespace std;

#pragma warning(disable :4786)

int XK_StringToInt( const string& str );

void XK_IntToString( string& str , int number );

#define XK_SAFE_RELEASE(pointer) {if(pointer){ delete(pointer);(pointer)=NULL; } }

#endif /* __PACMANDEMO_XKMISCTOOLS_H__ */
