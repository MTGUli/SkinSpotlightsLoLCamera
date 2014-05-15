#ifndef _MEMORYEDITOR_H_
#define _MEMORYEDITOR_H_

#include <process.h>
#include <windows.h>
#include <TlHelp32.h>

class MemoryEditor{
private:
	bool bGameFound;
	bool bRotation;
	bool bPosition;
	bool bZ;
	DWORD dwPID;
	LONG BASE;
	LONG ENTRY;
	DWORD FoVAddr;
	DWORD YRotationAddr;
	DWORD XRotationAddr;
	DWORD YPositionAddr;
	DWORD XPositionAddr;
	DWORD ZPositionAddr;

	float defaultY;
	float defaultX;
	float defaultFoV;

	float XRot;
	float YRot;
	float FoV;

	unsigned long MemoryEditor::dwFindPattern(HANDLE hProcess, unsigned long dwAddress, unsigned long dwLength, unsigned char* pbMask, char* pszString);
	bool CompareData(const unsigned char* pbData, const unsigned char* pbMask, const char* pszString);
	void WriteMem(int ADDR, int VALUE, HANDLE hProcess);
	void WriteMemFloat(int ADDR, float VALUE, HANDLE hProcess);
	int ReadMem(int Address, HANDLE hProcess);
	float ReadMemFloat(int Address, HANDLE hProcess);
	DWORD ReadMemDword(int Address, HANDLE hProcess);
public:
	MemoryEditor::MemoryEditor();
	unsigned __stdcall FindGame(const char* GameName);
	void WriteMemory();
	void WritePositionMemory(float x, float y);
	void WriteHeightMemory(float z);

	bool getGame(){return bGameFound;};
	DWORD getProcID(){return dwPID;};
	void setGame(bool b){bGameFound = b;};

	void setXRot(float f){XRot = f;};
	void setYRot(float f){YRot = f;};
	void setFoV(float f){FoV = f;};

	float getXRot(){return XRot;};
	float getYRot(){return YRot;};
	float getFoV(){return FoV;};

	float getX();
	float getY();
	float getZ();

	void modXRot(float f){
		XRot += f;
		if(XRot > 360)
			XRot -= 360;
		else if(XRot < 0)
			XRot +=360;
	};
	void modYRot(float f){
		YRot += f;

		if(YRot > 360)
			YRot -= 360;
		else if(YRot < 0)
			YRot+= 360;
	};
	void modFoV(float f){
		FoV += f;
	};

	void Reset(){
		XRot = defaultX;
		YRot = defaultY;
		FoV = defaultFoV;

		WriteMemory();
	};
};

#endif