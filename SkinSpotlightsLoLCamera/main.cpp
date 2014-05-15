#define USING_WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <sstream>
#pragma comment(lib, "winmm.lib")

//Defines:
#define LOL_WINDOW "League of Legends (TM) Client"
#include "MemoryEditor.h"
#include "Stopwatch.h"

#include "..\\LuaPlusLib\\LuaPlus.h"
#ifdef _DEBUG
	#pragma comment( lib, "..\\Debug\\LUAPlus.lib")
#else
	#pragma comment( lib, "..\\Release\\LUAPlus.lib")
#endif

LuaPlus::LuaState* pState;
LuaPlus::LuaObject globals;
char pPath[ MAX_PATH ];

typedef struct Mouse
{
	long x,y;
}Mouse;

typedef struct Keys{
	bool XRotDown, XRotUp;
	bool YRotLeft, YRotRight;
	bool FoVInc, FoVDec;
	bool Reset;
	bool RotSpeedUp, RotSpeedDown;
	bool MouseLeft, MouseRight, MouseTriggered;
	bool RunLUA, LUARunning;
	bool RaiseCamera, LowerCamera;
} Keys;

typedef struct diff{
	float x, y;
}diff;

Mouse MousePos;
Keys HotKeys;
diff AxisDiff;
const char g_szClassName[] = "LoLCameraTool-SkinSpotlights";
HHOOK hHook;
HHOOK hMouseHook;
MemoryEditor Editor;

float RotateSpeed = 28.5f;

LRESULT CALLBACK KeyboardProc(int nCode,WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MouseProc(int nCode,WPARAM wParam, LPARAM lParam);
void handleCamera(float dt);
HANDLE hThread;

//LUA FUNCTIONS:
int Print( LuaPlus::LuaState* pState )
{
	// Get the argument count
	int top = pState->GetTop();

	std::stringstream output;
	for( int i = 1; i <= top; ++i )
	{
		// Retrieve all arguments, if possible they will be converted to strings
		output << pState->CheckString(i) << std::endl;
	}
	std::string s = output.str();
	OutputDebugString(s.c_str());

	// We don't return any values to the script
	return 0;
}

// Direct registered function
void Finish()
{
	HotKeys.RunLUA = false;
}

// Registered function MUST always have this signature (int (funcName) (LuaPlus::LuaState*))
int SetFoV( LuaPlus::LuaState* pState )
{
	// Get the argument count
	//int top = pState->GetTop();

	std::stringstream output;
	//for( int i = 1; i <= top; ++i )
	//{
		// Retrieve all arguments, if possible they will be converted to strings
		output << pState->CheckString(1) << std::endl;
	//}
	float f;
	std::istringstream(output.str()) >> f;
	Editor.setFoV(f);
	Editor.WriteMemory();
	// We don't return any values to the script
	return 0;
}

//Force hotkeys on till it reaches the point it needs to be at
//Need to set Targets and set speeds
int RotateUp( LuaPlus::LuaState* pState) 
{
	std::stringstream output;
	float f;
	float deltaTick;
	float ScriptRotateSpeed;

	output << pState->CheckString(1) << std::endl;
	std::istringstream(output.str()) >> f;
	deltaTick = f;
	deltaTick *= 0.001f;
	output.str("");
	output << pState->CheckString(2) << std::endl;
	std::istringstream(output.str()) >> f;
	ScriptRotateSpeed = f;

	Editor.modXRot(deltaTick * ScriptRotateSpeed);
	Editor.WriteMemory();

	return 0;
}
//Pass in TICK and Rotate Speed
int RotateDown( LuaPlus::LuaState* pState )
{
	std::stringstream output;
	float f;
	float deltaTick;
	float ScriptRotateSpeed;

	output << pState->CheckString(1) << std::endl;
	std::istringstream(output.str()) >> f;
	deltaTick = f;
	deltaTick *= 0.001f;
	output.str("");
	output << pState->CheckString(2) << std::endl;
	std::istringstream(output.str()) >> f;
	ScriptRotateSpeed = f;

	Editor.modXRot(-(deltaTick * ScriptRotateSpeed));
	Editor.WriteMemory();

	return 0;
}
//Pass in TICK and Rotate Speed
int RotateLeft( LuaPlus::LuaState* pState )
{
	std::stringstream output;
	float f;
	float deltaTick;
	float ScriptRotateSpeed;

	output << pState->CheckString(1) << std::endl;
	std::istringstream(output.str()) >> f;
	deltaTick = f;
	deltaTick *= 0.001f;
	output.str("");
	output << pState->CheckString(2) << std::endl;
	std::istringstream(output.str()) >> f;
	ScriptRotateSpeed = f;

	Editor.modYRot(deltaTick * ScriptRotateSpeed);
	Editor.WriteMemory();

	return 0;
}

//Pass in TICK and Rotate Speed
int RotateRight( LuaPlus::LuaState* pState )
{
	std::stringstream output;
	float f;
	float deltaTick;
	float ScriptRotateSpeed;

	output << pState->CheckString(1) << std::endl;
	std::istringstream(output.str()) >> f;
	deltaTick = f;
	deltaTick *= 0.001f;

	output.str("");
	output << pState->CheckString(2) << std::endl;
	std::istringstream(output.str()) >> f;
	ScriptRotateSpeed = f;

	Editor.modYRot(-(deltaTick * ScriptRotateSpeed));
	Editor.WriteMemory();

	return 0;
}

//Pass in an Int for sleep
int SleepThread(LuaPlus::LuaState* pState){
	std::stringstream output;
	output << pState->CheckString(1) << std::endl;
	int i;
	std::istringstream(output.str()) >> i;
	Sleep(i);
	return 0;
}

//Pass in X and Y
int SetPosition( LuaPlus::LuaState* pState )
{
	std::stringstream output;
	float f;
	float x;
	float y;

	output << pState->CheckString(1) << std::endl;
	std::istringstream(output.str()) >> f;
	x = f;
	output.str("");
	output << pState->CheckString(2) << std::endl;
	std::istringstream(output.str()) >> f;
	y = f;

	Editor.WritePositionMemory(x, y);
	return 0;
}

//Pass in X and Y we want to move towards, speed and Tick
int MoveTowardsPosition( LuaPlus::LuaState* pState )
{
	std::stringstream output;
	float f;
	float x;
	float y;
	float moveSpeed;
	float deltaTick;

	output << pState->CheckString(1) << std::endl;
	std::istringstream(output.str()) >> f;
	x = f;
	output.str("");
	output << pState->CheckString(2) << std::endl;
	std::istringstream(output.str()) >> f;
	y = f;
	output.str("");
	output << pState->CheckString(3) << std::endl;
	std::istringstream(output.str()) >> f;
	moveSpeed = f;
	output.str("");
	output << pState->CheckString(4) << std::endl;
	std::istringstream(output.str()) >> f;
	deltaTick = f;
	deltaTick *= 0.001f;

	if(deltaTick == 0){
		return 0;
	}

	//DeltaTick at 0 messes up stuff, no idea why

	float CamX = Editor.getX();
	float CamY = Editor.getY();

	float dirX = x - CamX;
	float dirY = x - CamY;

	float hyp = sqrt(dirX*dirX + dirY*dirY);

	dirX /= hyp;
	dirY /= hyp;

	float xSpeed = dirX * moveSpeed * deltaTick;
	float ySpeed = dirY * moveSpeed * deltaTick;

	float xLoc = CamX + xSpeed;
	float yLoc = CamY + ySpeed;

	if(xSpeed < 0){
		if(xLoc < x){
			xLoc = x;
		}
	}else if(xSpeed > 0){
		if(xLoc > x){
			xLoc = x;
		}
	}

	if(ySpeed < 0){
		if(yLoc < y){
			yLoc = y;
		}
	}else if(ySpeed > 0){
		if(yLoc > y){
			yLoc = y;
		}
	}

	if(x == CamX && y == CamY){
		pState->PushBoolean(true);
	}else{
		Editor.WritePositionMemory(xLoc, yLoc);
		pState->PushBoolean(false);
	}

	return 1;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_CLOSE:
			UnhookWindowsHookEx(hHook);
			UnhookWindowsHookEx(hMouseHook);
			DestroyWindow(hwnd);
			break;

		case WM_DESTROY:
			UnhookWindowsHookEx(hHook);
			UnhookWindowsHookEx(hMouseHook);
			PostQuitMessage(0);
			break;

	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK DlgProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		switch(wParam)
		{
		case IDOK:
			EndDialog(hWndDlg, 0);
			return TRUE;
		}
		break;
	}

	return FALSE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;
	HWND hwnd;
	MSG Msg;

	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.style		 = 0;
	wc.lpfnWndProc	 = WndProc;
	wc.cbClsExtra	 = 0;
	wc.cbWndExtra	 = 0;
	wc.hInstance	 = hInstance;
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = g_szClassName;
	wc.hIconSm		 = LoadIcon(NULL, IDI_APPLICATION);

	if(!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		g_szClassName,
		"SkinSpotlights LoL Camera Tool",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
		NULL, NULL, hInstance, NULL);

	if(hwnd == NULL)
	{
		MessageBox(NULL, "Window Creation Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	//Hook Keyboard & Mouse
	hHook = SetWindowsHookEx(WH_KEYBOARD_LL,KeyboardProc,hInstance,NULL);
	hMouseHook  = SetWindowsHookEx(WH_MOUSE_LL,MouseProc,hInstance,NULL);

	//Register LUA Stuff:
	// Create a new lua state and load all default lua libraries
	pState = LuaPlus::LuaState::Create( true );

	// Get the global object of our state so that we can register global functions
	globals = pState->GetGlobals();

	// Register our Finish() function directly to lua
	globals.RegisterDirect("Finish",Finish);
	// Register our SetFoV() function to lua
	globals.Register("SetFoV",SetFoV);
	globals.Register("RotateRight",RotateRight);
	globals.Register("RotateLeft",RotateLeft);
	globals.Register("RotateUp",RotateUp);
	globals.Register("RotateDown",RotateDown);
	globals.Register("Print",Print);
	globals.Register("Sleep",SleepThread);
	globals.Register("Position", SetPosition);
	globals.Register("MoveTowardsPosition", MoveTowardsPosition);

	//Create our Stopwatch
	StopWatch stopWatch( pState );

	// Just run our "test.lua" file here
	GetCurrentDirectory(MAX_PATH,pPath);
	strcat_s(pPath,MAX_PATH,"\\Automate.lua");
/*
	if( pState->DoFile(pPath) )
	{
		// An error occured
		if( pState->GetTop() == 1 ){
			Print(pState);
		}
	}
*/
	LONG old = 0;
	LONG newTime = timeGetTime();
	DWORD g_msGameTickTime = 1000/500;
	DWORD g_msNextGameCall = GetTickCount() + g_msGameTickTime;

	while(1)
	{
		if (PeekMessage(&Msg,NULL,0,0,PM_REMOVE))
		{
		    if (Msg.message==WM_QUIT){
				UnhookWindowsHookEx(hHook);
				UnhookWindowsHookEx(hMouseHook);
				break;
			}
			TranslateMessage (&Msg);							
			DispatchMessage (&Msg);
		}

		DWORD ms = GetTickCount();
        DWORD msNext = g_msNextGameCall;
        LONG  lWait = 0;
        DWORD dwRet = WAIT_TIMEOUT;

		old = newTime;
		newTime = timeGetTime();
		float deltaTime = ((newTime - old) * 0.001f);

		if (ms < msNext)
            lWait = min((LONG)g_msGameTickTime, (LONG)(msNext - ms));

		if (lWait <= 1)
        {
            g_msNextGameCall = ms + g_msGameTickTime;
            handleCamera(deltaTime);
		}
        else
		{
			//Can cause lock up for some reason
            if (WAIT_TIMEOUT == MsgWaitForMultipleObjects (0, NULL, FALSE, lWait, QS_ALLEVENTS))
			{
               g_msNextGameCall = GetTickCount() + g_msGameTickTime;
               handleCamera(deltaTime);
			}
		}
	}
	return Msg.wParam;
}

unsigned __stdcall  RunLUA(void *data){
	if( pState->DoFile(pPath) )
	{
		// An error occured
		if( pState->GetTop() == 1 ){
			Print(pState);
		}
	}
	return 0;
}

void handleCamera(float dt){
	//Main Code:
	Editor.FindGame(LOL_WINDOW);
	if(Editor.getGame()){
		//Game Exists!
		if(HotKeys.XRotDown){
			Editor.modXRot(-(RotateSpeed * dt));
			Editor.WriteMemory();
		}
		if(HotKeys.XRotUp){
			Editor.modXRot((RotateSpeed * dt));
			Editor.WriteMemory();
		}
		if(HotKeys.YRotLeft){
			Editor.modYRot(-(RotateSpeed * dt));
			Editor.WriteMemory();
		}
		if(HotKeys.YRotRight){
			Editor.modYRot((RotateSpeed * dt));
			Editor.WriteMemory();
		}
		if(HotKeys.Reset){
			Editor.Reset();
			HotKeys.Reset = false;
		}
		if(HotKeys.FoVDec){
			Editor.modFoV(-(RotateSpeed * dt));
			Editor.WriteMemory();
		}
		if(HotKeys.FoVInc){
			Editor.modFoV((RotateSpeed * dt));
			Editor.WriteMemory();
		}
		if(HotKeys.RotSpeedUp){
			RotateSpeed += 2.5f * dt;
		}
		if(HotKeys.RotSpeedDown){
			RotateSpeed -= 2.5f * dt;
		}

		if(HotKeys.RaiseCamera){
			Editor.WriteHeightMemory(Editor.getZ() + (30.0f * dt));
		}

		if(HotKeys.LowerCamera){
			Editor.WriteHeightMemory(Editor.getZ() - (30.0f * dt));
		}

		if(HotKeys.MouseTriggered){
			Editor.modXRot(AxisDiff.y * dt * RotateSpeed);
			Editor.modYRot(AxisDiff.x * dt * RotateSpeed);
			Editor.WriteMemory();
			HotKeys.MouseTriggered = false;
		}
		if(HotKeys.RunLUA && !HotKeys.LUARunning){
			HotKeys.LUARunning = true;
			hThread = (HANDLE) _beginthreadex(NULL,0, RunLUA,NULL,0,NULL);
		}else if(!HotKeys.RunLUA && HotKeys.LUARunning){
			//Cancel the LUA
			TerminateThread(hThread, 0);
			CloseHandle(hThread);
			HotKeys.LUARunning = false;
		}
	}
}

LRESULT CALLBACK KeyboardProc(int nCode,WPARAM wParam, LPARAM lParam)
{
	if(wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
	{	
		PKBDLLHOOKSTRUCT pKey = (PKBDLLHOOKSTRUCT)lParam;
		if(pKey->vkCode == VK_NUMPAD2){
			HotKeys.XRotDown = true;
		}
		if(pKey->vkCode == VK_NUMPAD8){
			HotKeys.XRotUp = true;
		}
		if(pKey->vkCode == VK_NUMPAD4){
			HotKeys.YRotLeft = true;
		}
		if(pKey->vkCode == VK_NUMPAD6){
			HotKeys.YRotRight = true;
		}
		if(pKey->vkCode == VK_NUMPAD7){
			HotKeys.FoVDec = true;
		}
		if(pKey->vkCode == VK_NUMPAD9){
			HotKeys.FoVInc = true;
		}
		if(pKey->vkCode == VK_NUMPAD1){
			HotKeys.RotSpeedDown = true;
		}
		if(pKey->vkCode == VK_NUMPAD3){
			HotKeys.RotSpeedUp = true;
		}
		if(pKey->vkCode == VK_NUMPAD5){
			HotKeys.Reset = true;
		}
		if(pKey->vkCode == VK_ADD){
			HotKeys.RaiseCamera = true;
		}
		if(pKey->vkCode == VK_SUBTRACT){
			HotKeys.LowerCamera = true;
		}

		if(pKey->vkCode == VK_NUMPAD0){
			if(HotKeys.RunLUA){
				HotKeys.RunLUA = false;
			}else{
				HotKeys.RunLUA = true;
			}
		}
	}
	else if(wParam == WM_KEYUP || wParam == WM_SYSKEYUP)
	{	
		PKBDLLHOOKSTRUCT pKey = (PKBDLLHOOKSTRUCT)lParam;
		if(pKey->vkCode == VK_NUMPAD2){
			HotKeys.XRotDown = false;
		}
		if(pKey->vkCode == VK_NUMPAD8){
			HotKeys.XRotUp = false;
		}
		if(pKey->vkCode == VK_NUMPAD4){
			HotKeys.YRotLeft = false;
		}
		if(pKey->vkCode == VK_NUMPAD6){
			HotKeys.YRotRight = false;
		}
		if(pKey->vkCode == VK_NUMPAD7){
			HotKeys.FoVDec = false;
		}
		if(pKey->vkCode == VK_NUMPAD1){
			HotKeys.RotSpeedDown = false;
		}
		if(pKey->vkCode == VK_NUMPAD3){
			HotKeys.RotSpeedUp = true;
		}			
		if(pKey->vkCode == VK_NUMPAD9){
			HotKeys.FoVInc = false;
		}

		if(pKey->vkCode == VK_ADD){
			HotKeys.RaiseCamera = false;
		}

		if(pKey->vkCode == VK_SUBTRACT){
			HotKeys.LowerCamera = false;
		}

	}
	CallNextHookEx(hHook,nCode,wParam,lParam);
	return 0;
}

LRESULT CALLBACK MouseProc(int nCode,WPARAM wParam, LPARAM lParam)
{
	MOUSEHOOKSTRUCT * pMouseStruct = (MOUSEHOOKSTRUCT *)lParam;
    if (pMouseStruct != NULL){
		PKBDLLHOOKSTRUCT pKey = (PKBDLLHOOKSTRUCT)lParam;

		//Check to see if requirements are met to disable mouse and check movement:
		if(WM_LBUTTONDOWN == wParam){
			HotKeys.MouseLeft = true;
		}else if(WM_LBUTTONUP == wParam){
			HotKeys.MouseLeft = false;
		}

		if(WM_RBUTTONDOWN == wParam){
			HotKeys.MouseRight = true;
		}else if(WM_RBUTTONUP == wParam){
			HotKeys.MouseRight = false;
		}

		if(HotKeys.MouseRight && HotKeys.MouseLeft){
			HotKeys.MouseTriggered = true;
			AxisDiff.x = pMouseStruct->pt.x - MousePos.x;
			AxisDiff.y = pMouseStruct->pt.y - MousePos.y;
			return -1;
		}
		
		//Mouse isnt required right now so keep positions updated
		MousePos.x = pMouseStruct->pt.x;
		MousePos.y = pMouseStruct->pt.y;
	}
	CallNextHookEx(hMouseHook,nCode,wParam,lParam);
	return 0;
}