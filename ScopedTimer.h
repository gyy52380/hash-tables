#pragma once

#include "typedef.h"
#include <intrin.h>
#include <windows.h>

struct Scoped_Timer_Instructions
{
	u64* global_store;
	u64 start_time;

	Scoped_Timer_Instructions(u64* global_store)
		: global_store(global_store)
	{
		start_time = __rdtsc();
	}

	~Scoped_Timer_Instructions()
	{
		u64 end_time = __rdtsc();
		u64 duration = end_time - start_time;
		*global_store += duration;
	}
};

#define TIMED_BLOCK_INSTRUCTIONS(store) Scoped_Timer_Instructions __timer_rdtsc(&store);

struct Scoped_Timer
{
	u64* global_store;

	LARGE_INTEGER time1, time2, frequency;

	Scoped_Timer(u64* global_store)
		: global_store(global_store)
	{
		QueryPerformanceCounter(&time1);
	}

	~Scoped_Timer()
	{
		QueryPerformanceCounter(&time2);
		QueryPerformanceFrequency(&frequency);
		u64 milliseconds = (u64)((double)(time2.QuadPart - time1.QuadPart) / (double)frequency.QuadPart * 1000 + 0.5f);
		*global_store += milliseconds;
	}
};

#define TIMED_BLOCK(store) Scoped_Timer __timer(&store);