/************************************************************************
 *	@file		：	PacManDemo/XKTimer.cpp
 *	@brief		：	
 *
 *	@author		：	Xiong Xinke
 *	@copyright	：	Xiong Xinke
 *	@date		：	2005-2-23 23:53:37
 ************************************************************************/

#include "XKTimer.h"
#include <mmsystem.h>

bool XKTimer::m_sbPaused = false;

XKTimer::XKTimer()
{   	
	Reset();
}

void XKTimer::Update() 
{    
	m_bElapsedCached = false;

	//更新记数值
	++m_nFrameCount;

	//更新时钟的状态
	QueryPerformanceCounter( (LARGE_INTEGER *)  &m_nStartTick);
	m_nElapsed = m_nStartTick - m_nLastTick;                
	m_nLastTick = m_nStartTick;
	
	//如果暂停就先不计时
	if( m_sbPaused ) 
		return;

	//消逝的时间增加
	m_nTime += m_nElapsed;		
}

/**
 * @name  XKTimer::Reset
 * @brief     重置时钟
 * @return    void 
 * @remark    
 **/
void XKTimer::Reset()
{  
	QueryPerformanceFrequency( (LARGE_INTEGER *) &m_nFrequency);	// ticks per second
	QueryPerformanceCounter( (LARGE_INTEGER *)  &m_nStartTick);	// where we're at now...

	m_nLastTick = m_nStartTick;										// is where we began

	m_nTime			= 0;	
	m_nElapsed		 = 0;    
	m_fElapsedCache  = 0.0f;
	m_bElapsedCached = false;
	m_nFrameCount	  = 0;
}

/**
 * @name  XKTimer::GetFPS
 * @brief     获取帧速率
 * @return    float 
 * @remark    
 **/
float XKTimer::GetFPS() const
{
	return m_nElapsed ? 1/((float)m_nElapsed/m_nFrequency) : 0;
}

/**
 * @name  XKTimer::TimeMod
 * @brief     
 * @return    float 
 * @param      const float& fInterval
 * @param     float fStartTime
 * @remark    
 **/
float XKTimer::TimeMod( const float& fInterval, float fStartTime) 
{    
	__int64 start_time = (__int64)(fStartTime * m_nFrequency);
	__int64 interval = (__int64)(fInterval * m_nFrequency);

	if (start_time == NULL)
	{
		start_time = m_nTime;
	}
	else
	{
		start_time = m_nTime - start_time;
	}

	if( start_time % interval <(start_time  - m_nElapsed) % interval)
	{
		return 0;
	}    
	return (start_time % interval) / (float)m_nFrequency;
}

/**
 * @name  XKTimer::GetTime
 * @brief     
 * @return    float 
 * @remark    
 **/
float XKTimer::GetTime() const 
{ 
	return m_nTime / (float)m_nFrequency; 
}

/**
 * @name  XKTimer::GetElapsed
 * @brief     
 * @return    float 
 * @remark    
 **/
float XKTimer::GetElapsed() 
{ 
	if (!m_bElapsedCached) 
	{
		m_fElapsedCache = m_nElapsed / (float)m_nFrequency;
		m_bElapsedCached = true;
	}
	return m_fElapsedCache; 
}

/**
 * @name  XKTimer::GetTick
 * @brief     
 * @return    int 
 * @remark    
 **/
int XKTimer::GetTick() const 
{ 
	return (int)m_nLastTick; 
}

/**
 * @name  XKTimer::GetFrameCount
 * @brief     
 * @return    int 
 * @remark    
 **/
int XKTimer::GetFrameCount() const 
{ 
	return m_nFrameCount; 
}