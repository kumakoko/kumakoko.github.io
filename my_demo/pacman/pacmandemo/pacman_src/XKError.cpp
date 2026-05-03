/************************************************************************
 *	@file		：	PacManDemo/XKError.cpp
 *	@brief		：	
 *
 *	@author		：	Xiong Xinke
 *	@copyright	：	Xiong Xinke
 *	@date		：	2005-2-22 21:37:37
 ************************************************************************/

#include "XKError.h"
#include "XKGlobals.h"
#include "XKMiscTools.h"
#include <tchar.h>
#include <d3dx8.h>
#include <dxerr8.h>

/**
 * @name  XKError::XKError
 * @brief     
 * @return    
 * @param     const string& message
 * @remark    
 **/
XKError::XKError( const string& message ) :
m_strTitle( _T("异常") ),
m_eType(EXCEPTION_MESSAGE),
m_strMessage(message)
{

}

/**
 * @name  XKError::XKError
 * @brief     
 * @return    
 * @param     const string& message
 * @param     char* file
 * @param     DWORD line
 * @remark    
 **/
XKError::XKError(const string& message , char* file , DWORD line):
m_strTitle( _T("异常") ),
m_eType(EXCEPTION_REGULAR),
m_strMessage(message),
m_dwLine(line)
{
	m_strFile = file;
}

/**
 * @name  XKError::XKError
 * @brief     
 * @return    
 * @param      const string& message
 * @param     char* file
 * @param     DWORD line
 * @param     HRESULT hr
 * @remark    
 **/
XKError::XKError( const string& message, char* file, DWORD line, HRESULT hr ):
m_strTitle( _T("异常") ),
m_eType(EXCEPTION_REGULAR),
m_strMessage(message),
m_dwLine(line),
m_hResult(hr)
{
	m_strFile = file;
}

XKError::~XKError()
{
	
}

/**
 * @name  XKError::AssembleOutput
 * @brief     把要显示到error message box上的字符串汇集起来
 * @return    string 
 * @remark    
 **/
string XKError::AssembleOutput() const
{
	string output(m_strMessage);

	switch( m_eType )
	{
	case EXCEPTION_DX:
		output += _T("\n 出现了DirectX错误：\n");
		output += DXGetErrorString8(m_hResult);
		break;
	case EXCEPTION_REGULAR:
		{
			string line_str;
			output += _T("\n 文件名：");
			output += m_strFile;
			output += _T("\n 行号： ");
			XK_IntToString(line_str , m_dwLine);
			output += line_str;
		}
		break;
	case EXCEPTION_MESSAGE:
	default:
		break;
	}

	return output;
}


/**
 * @name  XKError::Notify
 * @brief     
 * @return    void 
 * @remark    
 **/
void XKError::Notify() const
{
	string output = AssembleOutput();
	ShowCursor(TRUE);
	MessageBox( g_pGame->GetSafeHWND() , output.c_str() ,
				"异常", MB_OK | MB_ICONERROR );
}