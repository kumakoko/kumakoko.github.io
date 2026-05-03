/************************************************************************
 *	@file		£º	PacManDemo/XKMiscTools.cpp
 *	@brief		£º	
 *
 *	@author		£º	Xiong Xinke
 *	@copyright	£º	Xiong Xinke
 *	@date		£º	2005-2-22 22:56:58
 ************************************************************************/

#include "XKMiscTools.h"

#pragma warning(disable :4786)

/**
 * @name  XK_StringToInt
 * @brief     
 * @return    int 
 * @param     const string& str
 * @remark    
 **/
int XK_StringToInt( const string& str )
{
	return atoi( str.c_str() );
}

/**
 * @name  XK_IntToString
 * @brief     
 * @return    void 
 * @param      string& str
 * @param     int number
 * @remark    
 **/
void XK_IntToString( string& str , int number )
{
	char number_tmp[256];
	memset( number_tmp , NULL , sizeof(char)*256 );
	itoa( number , number_tmp , 10 );
	str = number_tmp;
}

// -------------------------------------------------------------------------
