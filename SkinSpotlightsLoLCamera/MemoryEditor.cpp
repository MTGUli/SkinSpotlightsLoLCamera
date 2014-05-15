#include "MemoryEditor.h"

MemoryEditor::MemoryEditor(){
	bGameFound = false;
	BASE = 0;
	ENTRY = 0;
	YRot = 180.0f;
	XRot = 56.0f;
	FoV = 50.0f;
}

unsigned __stdcall MemoryEditor::FindGame(const char* GameName){
	HWND hWnd = NULL;
	DWORD dwLocalPID = 0;

	if ((hWnd = FindWindow(NULL, GameName)) == NULL)
	{
		//Game Not Found
		bGameFound = false;
		return 1;
	}
		
	//Get Windows Process ID
	GetWindowThreadProcessId(hWnd, &dwLocalPID);

	if(dwLocalPID != dwPID){
		bPosition = true;
		bRotation = true;
		bZ = true;
		//New PID
		dwPID = dwLocalPID;
		HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);

		//Failed to Open process for some reason
		if (!hProc)
		{
			//Failed to Open Process, Need to run as admin?
			bGameFound = false;
			return 1;
		}

		LPVOID lpMemory = NULL;
		DWORD dwOldProtect = 0;
		DWORD dwWritten = 0;

		//Base Offset Stuff:
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID); 
		MODULEENTRY32 ModuleEntry32;
		
		ModuleEntry32.dwSize = sizeof(MODULEENTRY32);
		Module32First(hSnapshot, &ModuleEntry32); 

		BASE = (LONG)ModuleEntry32.modBaseAddr;
		DWORD SIZE = ModuleEntry32.modBaseSize;
		CloseHandle(hSnapshot);

		//ENTRY = BASE + OFFSET;
		ENTRY = dwFindPattern(hProc, BASE, SIZE, (BYTE *)"\xF3\x0F\x10\x05\x00\x00\x00\x00\xF3\x0F\x11\x56\x04\xF3\x0F\x11\x46\x2C", "xxxx????xxxxxxxxxx");

		if(ENTRY == 0){
			//Error The Pattern is out of date
			bRotation = false;
			//return 1;
		}

		ENTRY = ENTRY + 0x04; //Move off by 4 to get the actual address offset
		DWORD MemAddr= ReadMemDword(ENTRY, hProc);
		FoVAddr = MemAddr;
		YRotationAddr = MemAddr - 0x04;
		XRotationAddr = MemAddr - 0x08;

		//ENTRY = BASE + OFFSET;
		ENTRY = dwFindPattern(hProc, BASE, SIZE, (BYTE *)"\xF3\x0F\x11\x0D\x00\x00\x00\x00\xF3\x0F\x11\x1D\x00\x00\x00\x00\xF3\x0F\x11\x25\x00\x00\x00\x00", "xxxx????xxxx????xxxx????");
		
		if(ENTRY == 0){
			//Error The Pattern is out of date
			bPosition = false;
			//return 1;
		}

		if(!bPosition && !bRotation){
			//Error The Pattern is out of date
			bGameFound = false;
			return 1;
		}

		ENTRY = ENTRY + 0x04; //Move off by 4 to get the actual address offset
		MemAddr = 0;
		MemAddr = ReadMemDword(ENTRY, hProc);

		XPositionAddr = MemAddr /*- 0x04*/;
		YPositionAddr = MemAddr + 0x08;

		//ENTRY = BASE + OFFSET;
		ENTRY = dwFindPattern(hProc, BASE, SIZE, (BYTE *)"\xC7\x05\x00\x00\x00\x00\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x8B\x15\x00\x00\x00\x00", "xx????????x????xx????");
		
		if(ENTRY == 0){
			//Error The Pattern is out of date
			bZ = false;
			//return 1;
		}

		/*
		01485F34

\xF3\x0F\x10\x40\x6C\xF3\x0F\x11\x05\x00\x00\x00\x00\xC7\x05\x00\x00\x00\x00\x00\x00\x00\x00
xxxxxxxxx????xx????????


\xC7\x05\x00\x00\x00\x00\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x8B\x15\x00\x00\x00\x00
xx????????x????xx????

01485F34   C705 D3B01E02 00>MOV DWORD PTR DS:[21EB0D3],43FA0000
01485F3E   E8 3DF9FFFF      CALL League_o.01485880
01485F43   8B15 CFB01E02    MOV EDX,DWORD PTR DS:[21EB0CF]
		*/

		ZPositionAddr = ENTRY + 0x06;

		defaultX = ReadMemFloat(XRotationAddr, hProc);
		defaultY = ReadMemFloat(YRotationAddr, hProc);
		defaultFoV = ReadMemFloat(FoVAddr, hProc);

		XRot = defaultX;
		YRot = defaultY;
		FoV = defaultFoV;

		CloseHandle(hProc);
		bGameFound = true;
	}

	return 0;
}

unsigned long MemoryEditor::dwFindPattern(HANDLE hProcess, unsigned long dwAddress, unsigned long dwLength, unsigned char* pbMask, char* pszString) 
{
    unsigned long dwResult = NULL;
    unsigned char* pbBuffer = (unsigned char*) malloc(dwLength);


    if(pbBuffer)
    {
        if(ReadProcessMemory(hProcess, (void*) dwAddress, pbBuffer, dwLength, NULL))
        {
            for(unsigned long i = 0; i < dwLength; i++) 
            {
                if(CompareData((BYTE*) (pbBuffer + i), pbMask, pszString)) 
                {
                    dwResult = (unsigned long)(dwAddress + i);
                    break;
                }
            }
        }


        free(pbBuffer);
        pbBuffer = NULL;
    }


    return dwResult; 
}

bool MemoryEditor::CompareData(const unsigned char* pbData, const unsigned char* pbMask, const char* pszString) 
{ 
    for( ; *pszString; ++pszString, ++pbData, ++pbMask) 
    {
        if(*pszString == 'x' && *pbData != *pbMask) 
            return FALSE; 
    }


    return (*pszString) == NULL; 
}

void MemoryEditor::WriteMem(int ADDR, int VALUE, HANDLE hProcess)
{
        WriteProcessMemory(hProcess, LPVOID(ADDR), &VALUE, sizeof(VALUE), NULL);
}

void MemoryEditor::WriteMemFloat(int ADDR, float VALUE, HANDLE hProcess)
{
        WriteProcessMemory(hProcess, LPVOID(ADDR), &VALUE, sizeof(VALUE), NULL);
}

int MemoryEditor::ReadMem(int Address, HANDLE hProcess)
{
        int Output = 0;
        ReadProcessMemory(hProcess, LPCVOID(Address), &Output, sizeof(Output), NULL);
        return Output;
}

float MemoryEditor::ReadMemFloat(int Address, HANDLE hProcess)
{
        float Output = 0;
        ReadProcessMemory(hProcess, LPCVOID(Address), &Output, sizeof(Output), NULL);
        return Output;
}

DWORD MemoryEditor::ReadMemDword(int Address, HANDLE hProcess){
	DWORD Output = 0;
	ReadProcessMemory(hProcess, LPCVOID(Address), &Output, sizeof(DWORD), NULL);
	return Output;
}

void MemoryEditor::WriteMemory(){
	if(bRotation){
		HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
		WriteMemFloat(XRotationAddr, XRot, hProc);
		WriteMemFloat(YRotationAddr, YRot, hProc);
		WriteMemFloat(FoVAddr, FoV, hProc);
		CloseHandle(hProc);
	}
}

void MemoryEditor::WritePositionMemory(float x, float y){
	if(bPosition){
		HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
		WriteMemFloat(XPositionAddr, x, hProc);
		WriteMemFloat(YPositionAddr, y, hProc);
		CloseHandle(hProc);
	}
}

void MemoryEditor::WriteHeightMemory(float z){
	if(bZ){
		HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
		WriteMemFloat(ZPositionAddr, z, hProc);
		CloseHandle(hProc);
	}
}

float MemoryEditor::getY(){
	float yPos = 0;
	if(bPosition){
		HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
		yPos = ReadMemFloat(YPositionAddr, hProc);
		CloseHandle(hProc);
	}
	return yPos;
}

float MemoryEditor::getX(){
	float xPos = 0;
	if(bPosition){
		HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
		xPos = ReadMemFloat(XPositionAddr, hProc);
		CloseHandle(hProc);
	}
	return xPos;
}

float MemoryEditor::getZ(){
	float zPos = 0;
	if(bPosition){
		HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
		zPos = ReadMemFloat(ZPositionAddr, hProc);
		CloseHandle(hProc);
	}
	return zPos;
}