/*
#############################################################################################
# Game: Gears of War 3
# TitleID: 4D5308AB
# Update: 6
# Credits: xecheats, gutumodz, shark.mods, unknowncheats
#############################################################################################
*/

#include <stdio.h>
#include <xtl.h>
#include <xboxmath.h>
#include "kernel.h"

DWORD TitleID = 0x4D5308AB;
DWORD GObjects = 0x8343D188; //(DWORD)GetModuleHandle("default.exe")
DWORD GName = 0x8343D148;//(DWORD)GetModuleHandle("default.exe")
DWORD NamePos = 0x2C;
 
template < class T > struct TArray
{
	T* Data;
	DWORD Num;
	DWORD Max;
};
 
struct UObject
{
	UCHAR Unknown[0x2C]; //unknowed data
	DWORD NameIndex; //struct FName
};
 
struct FNameEntry
{
	UCHAR Unknown[0x10];
	char Name[1];
};

TArray< UObject* >* GObjObjects = ( TArray< UObject* >* ) GObjects; //global objects
TArray< FNameEntry* >* Names = ( TArray< FNameEntry* >* ) GName; //global names
 
PCHAR GetName ( UObject* Object )
{
	DWORD NameIndex = *(PDWORD) ( (DWORD) Object + NamePos );
 
	if ( NameIndex < 0 || NameIndex > Names->Num )
	{
		static char ret[256];
		sprintf_s ( ret, "INVALID NAME INDEX : %i > %i", NameIndex, Names->Num );
		return ret;
	}
	else
	{
		return Names->Data[ NameIndex ]->Name;
	}
}
 
void ObjectDump()
{
	FILE* Log = NULL;      
	fopen_s ( &Log, "ObjectDump.txt", "w+" );
       
	for ( DWORD i = 0x0; i < GObjObjects->Num; i++ )
	{
		if ( ! GObjObjects->Data[ i ] ) { continue; }
			fprintf ( Log, "UObject[%06i] %-50s 0x%X\n", i, GetName ( GObjObjects->Data[ i ] ), GObjObjects->Data[ i ] );
	}

	fclose ( Log );
}
 
void NameDump()
{
	FILE* Log = NULL;      
	fopen_s ( &Log, "NameDump.txt", "w+" );
       
	for ( DWORD i = 0x0; i < Names->Num; i++ )
	{
		if ( ! Names->Data[ i ] ) { continue; }
			fprintf ( Log, "Name[%06i] %s\n", i, Names->Data[ i ]->Name );
	}
 
	fclose ( Log );
}

UINT32 resolveFunct(char* modname, UINT32 ord) 
{ 
	UINT32 ptr32_0 = 0, ret = 0, ptr2 = 0; 
	ret = XexGetModuleHandle(modname, (PHANDLE)&ptr32_0); 
	if(ret == 0){ 
		ret = XexGetProcedureAddress((HANDLE)ptr32_0, ord, &ptr2); 
		if(ptr2 != 0) 
			return(ptr2); 
	} 
	return(0); 
}
 
void onAttach()
{
	UINT32 (*XamGetCurrentTitleId)(void) = XamGetCurrentTitleId = (UINT32 (__cdecl *)(void))(resolveFunct("xam.xex", 0x1CF));
	
	for(;;)
	{
		if(XamGetCurrentTitleId() == TitleID)
		{ 
			Sleep (60000);
			NameDump();
			ObjectDump();
			printf("Dump done!");
		}
	}
}

bool TrayOpen()
{
	char message[0x10];
	char reply[0x10];
	message[0] = 0xA;
	HalSendSMCMessage( message, reply );
	return reply[1] == 0x60;
}

BOOL APIENTRY DllMain( HANDLE hModule, DWORD dwReason, LPVOID lpReserved ) 
{
	if ( dwReason == DLL_PROCESS_ATTACH ) 
	{
		if (!TrayOpen())
		{
			HANDLE hThread;
			DWORD dwThreadId;
			ExCreateThread( &hThread, 0, &dwThreadId, ( PVOID )XapiThreadStartup , ( LPTHREAD_START_ROUTINE )onAttach, 0, 0x2 );
			XSetThreadProcessor(hThread, 4);
			ResumeThread( hThread );
			CloseHandle( hThread );
		}
	}
	return TRUE;
}
