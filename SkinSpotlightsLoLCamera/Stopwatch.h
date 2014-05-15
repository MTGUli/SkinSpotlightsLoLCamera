#pragma once
#ifndef _STOPWATCH_H_
#define _STOPWATCH_H_

#include <Windows.h>
#include "..\\LuaPlusLib\\LuaPlus.h"
#ifdef _DEBUG
	#pragma comment( lib, "..\\Debug\\LUAPlus.lib")
#else
	#pragma comment( lib, "..\\Release\\LUAPlus.lib")
#endif
#include <iostream>
#include <string>
#include <sstream>
#pragma comment(lib, "winmm.lib")

class StopWatch
{
private:
	LONG old;
	LONG newTime;
	LONG deltaTimeMS;
	LONG totalTick;
	bool bEnabled;
public:
	StopWatch( LuaPlus::LuaState *pState );
	int Start( LuaPlus::LuaState *pState );
	int Stop( LuaPlus::LuaState *pState );
	int Index( LuaPlus::LuaState* pState );
};

#endif