#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include "shlwapi.h"
#include <iostream>


typedef int (__stdcall *pOldMBox)(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption,UINT uType);
pOldMBox pMBox = NULL;

INT_PTR
WINAPI
FakeDialogBoxParamW(
					__in_opt HINSTANCE hInstance,
					__in LPCWSTR lpTemplateName,
					__in_opt HWND hWndParent,
					__in_opt DLGPROC lpDialogFunc,
					__in LPARAM dwInitParam)
{
	std::cout<<"aaa"<<std::endl;
	return 1;
}


int __stdcall HookMBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption,UINT uType)
{
	if (NULL == pMBox)
	{
		return MessageBox(hWnd,lpText,lpCaption,uType);
	}
	else
	{
		std::cout<<"aaa"<<std::endl;
		return pMBox(NULL,"HHH! IAT  HOOK finishing£¡","HOOK",MB_OK);
	}
}

int ReAPI(const char* DllName, const char* FunName)
{
	 CHAR szPath[MAX_PATH + 1] = "C:\\Windows\\System32";
	 ::PathAppend(szPath, _T("IEFrame.dll"));
  	HMODULE hNetMgrDLL = LoadLibrary(szPath);
	 
	 if (hNetMgrDLL == NULL)
	 {
	 	return 0;
	 }
	unsigned long  (__stdcall *faddr)(void*);
	int t;
	size_t abc;
	HANDLE hp, ht;
	LPVOID paddr;
	unsigned long exitcode;
	int dllnamelen;

	HANDLE pBegin = GetModuleHandle(NULL);
	if (pBegin == NULL)
	{
		return 0;
	}

	PBYTE  pBegin2 = (PBYTE)pBegin;
	PIMAGE_DOS_HEADER DOS = PIMAGE_DOS_HEADER(pBegin2);
	PIMAGE_NT_HEADERS NT = PIMAGE_NT_HEADERS(pBegin2+DOS->e_lfanew);
	PIMAGE_OPTIONAL_HEADER OPTION = &(NT->OptionalHeader);
	PIMAGE_IMPORT_DESCRIPTOR IMPORT = PIMAGE_IMPORT_DESCRIPTOR(OPTION->DataDirectory[1].VirtualAddress + pBegin2);
	while (IMPORT->Name)
	{
		char* OurDllName = (char*)(IMPORT->Name + pBegin2);
		if (0 == strcmpi(DllName , OurDllName))
		{
			break;
		}
		IMPORT++;
	}

	PIMAGE_IMPORT_BY_NAME  pImportByName = NULL;
	PIMAGE_THUNK_DATA   pOriginalThunk = NULL;
	PIMAGE_THUNK_DATA   pFirstThunk = NULL;

	pOriginalThunk = (PIMAGE_THUNK_DATA)(IMPORT->OriginalFirstThunk + pBegin2);
	pFirstThunk = (PIMAGE_THUNK_DATA)(IMPORT->FirstThunk + pBegin2);

	while (pOriginalThunk->u1.Function) 
	{
		DWORD u1 = pOriginalThunk->u1.Ordinal;  
		if ((u1 & IMAGE_ORDINAL_FLAG) != IMAGE_ORDINAL_FLAG)  
		{
			pImportByName = (PIMAGE_IMPORT_BY_NAME)((DWORD)pOriginalThunk->u1.AddressOfData + pBegin2);
			char* OurFunName = (char*)(pImportByName->Name);
			
			if (0 == strcmpi(FunName,OurFunName))
			{
				MEMORY_BASIC_INFORMATION mbi_thunk;
				VirtualQuery(pFirstThunk, &mbi_thunk, sizeof(MEMORY_BASIC_INFORMATION));
				
				DWORD dwOLD;
				VirtualProtect(pFirstThunk,sizeof(DWORD),PAGE_READWRITE,&dwOLD);
				
				pMBox =(pOldMBox)(pFirstThunk->u1.Function);
				pFirstThunk->u1.Function = (ULONG_PTR)HookMBox;
			           
				VirtualProtect(pFirstThunk,sizeof(DWORD),dwOLD,0);
				break;
			}
		}
		pOriginalThunk++;
		pFirstThunk++;
	}

	return 0;
}

int main()
{
	 ReAPI("User32.dll","MessageBoxA");
	 MessageBoxA(NULL,"HOOK wrong","HOOK",MB_OK);

	return 0;
}