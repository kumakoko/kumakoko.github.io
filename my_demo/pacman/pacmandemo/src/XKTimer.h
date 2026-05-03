/************************************************************************
 *	@file		：	PacManDemo/XKTimer.h
 *	@brief		：	计时器
 *
 *	@author		：	Xiong Xinke
 *	@copyright	：	Xiong Xinke
 *	@date		：	2005-2-23 23:53:11
 ************************************************************************/
#ifndef __PACMANDEMO_XKTIMER_H__
#define __PACMANDEMO_XKTIMER_H__

#include <windows.h>

class XKTimer  
{
public:


	XKTimer();
	void	Update();
	void	Reset();
	float	TimeMod(const float& interval, float startTime = NULL);
	float	GetTime() const;
	float	GetElapsed();
	int		GetTick() const;
	float	GetFPS() const;
	int		GetFrameCount() const;

	static bool m_sbPaused;

private:
	__int64	m_nStartTick;	//开始计时的时刻
	__int64	m_nLastTick;    //上次update之后的时刻
	__int64	m_nFrequency;	//每秒钟的计时次数
	__int64	m_nElapsed;		//
	__int64	m_nTime;		//
	int		m_nFrameCount;		//
	bool	m_bElapsedCached;	//
	float	m_fElapsedCache;	//
};

#endif /* __PACMANDEMO_XKTIMER_H__ */
