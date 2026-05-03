 /************************************************************************
 *	@file		：	PacManDemo/XKError.h
 *	@brief		：	本demo的异常机制，XKError继承自STL的exception类
 *
 *	@author		：	Xiong Xinke
 *	@copyright	：	Xiong Xinke
 *	@date		：	2005-2-22 21:30:58
 ************************************************************************/
#ifndef __PACMANDEMO_XKERROR_H__
#define __PACMANDEMO_XKERROR_H__

#include <exception>
#include <string>
#include <windows.h>

using namespace std;

#pragma warning(disable : 4786)

class XKError : public exception  
{
public:
	XKError( const string& message );
	XKError( const string& message , char* file , DWORD line );
	XKError( const string& message, char* file, DWORD line, HRESULT hr);
	virtual ~XKError();

	void Notify() const;

private:

	string AssembleOutput() const;	

	//异常的类型，取决于构造一个XKError对象时使用哪种构造函数
	enum EXCEPTION_TYPE 
	{
		EXCEPTION_MESSAGE,	//只是弹出错误消息
		EXCEPTION_REGULAR,	//弹出错误消息，还带有文件名和错误所在行号
		EXCEPTION_DX		//弹出错误消息，还带有文件名和错误所在行号以及错误代码
	};

	EXCEPTION_TYPE m_eType;	//异常的类型
	string m_strTitle;		//弹出消息框的标题字符串
	string m_strMessage;	//弹出消息框的错误信息字符串
	string m_strFile;		//弹出消息框的错误所在的文件名的字符串
	DWORD  m_dwLine;		//弹出消息框的错误所在的行号的字符串
	HRESULT m_hResult;		//返回的错误代码
};
#endif /* __PACMANDEMO_XKERROR_H__ */
