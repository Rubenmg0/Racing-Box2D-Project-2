// ----------------------------------------------------
// j1Timer.cpp
// Fast timer with milisecons precision
// ----------------------------------------------------

#include "Timer.h"

#include "raylib.h"

Timer::Timer()
{
	Start();
}

void Timer::Start()
{
	started_at = GetTime();
	paused_at = 0;
	timePaused = false;
}

double Timer::ReadSec() const
{
	if (timePaused == true)
	{
		return paused_at;
	}
	if (timePaused == false)
	{
		return (GetTime() - started_at + paused_at);
	}
}

void Timer::Stop()
{
	if (timePaused == false)
	{
		paused_at = ReadSec();
	}
	timePaused = true;
}

void Timer::ContinueTimer()
{
	timePaused = false;
}