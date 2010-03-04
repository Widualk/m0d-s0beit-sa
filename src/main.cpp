/*

	PROJECT:		m0d_s0beit_sa
	LICENSE:		See LICENSE in the top level directory
	COPYRIGHT:		Copyright 2007, 2008, 2009, 2010 we_sux

	m0d_s0beit_sa is available from http://code.google.com/p/m0d-s0beit-sa/

	m0d_s0beit_sa is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	m0d_s0beit_sa is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with m0d_s0beit_sa.  If not, see <http://www.gnu.org/licenses/>.

	$LastChangedDate: 2010-01-05 01:40:59 -0600 (Tue, 05 Jan 2010) $
	$LastChangedBy: futnucks $
	$Revision: 43 $
	$HeadURL: https://m0d-s0beit-sa.googlecode.com/svn/trunk/src/main.cpp $
	$Id: main.cpp 43 2010-01-05 07:40:59Z futnucks $

*/

#include "main.h"


HINSTANCE   g_hOrigDll = NULL;
HMODULE		g_hDllModule = NULL;
char		g_szWorkingDirectory[MAX_PATH];
FILE		*g_flLog = NULL;
FILE		*g_flLogAll = NULL;
FILE		*g_flLogChatbox = NULL;
FILE		*g_flLogChatboxAll = NULL;
char		g_szLastFunc[256];
uint32_t	g_dwSAMP_Addr;

CSettingsSAInterface *g_pCSettingsSAInterface = (CSettingsSAInterface*)CLASS_CMenuManager;
D3DPRESENT_PARAMETERS *g_pGTAPresent = (D3DPRESENT_PARAMETERS*)0xC9C040;
RsGlobalType *g_RsGlobal = (RsGlobalType*)0xC17040;


// new half-assed CCamera init
CCameraSA g_CCamera;
int is_init_g_CCamera = false;
void init_g_CCamera(void)
{
	if (!is_init_g_CCamera)
	{
		traceLastFunc("init_g_CCamera()");
		g_CCamera.internalInterface = (CCameraSAInterface*)CLASS_CCamera;
		if (isBadPtr_writeAny(g_CCamera.internalInterface, sizeof(CCameraSAInterface))) return;
		for(int i = 0; i<MAX_CAMS;i++)
			g_CCamera.Cams[i].m_pInterface = (CCamSAInterface*)&g_CCamera.internalInterface->Cams[i];
		is_init_g_CCamera = true;
	}
}


void traceLastFunc(const char *szFunc)
{
	_snprintf_s(g_szLastFunc, sizeof(g_szLastFunc), szFunc);
}

void Log(const char *fmt, ...)
{
   SYSTEMTIME time;
   va_list ap;

   if(g_flLog == NULL)
   {
      char filename[512];
      snprintf(filename, sizeof(filename), "%s\\%s", g_szWorkingDirectory, "m0d_s0beit_sa.log");
	  
	  remove(filename);
      g_flLog = fopen(filename, "a");
      if(g_flLog == NULL) return;
   }

   GetLocalTime(&time);
   fprintf(g_flLog, "[%02d:%02d:%02d.%03d] ", time.wHour, time.wMinute, time.wSecond,time.wMilliseconds);
   va_start(ap, fmt);
   vfprintf(g_flLog, fmt, ap);
   va_end(ap);
   fprintf(g_flLog, "\n");
   fflush(g_flLog);

   if(g_flLogAll == NULL)
   {
      char filename_all[512];
	  snprintf(filename_all, sizeof(filename_all), "%s\\%s", g_szWorkingDirectory, "m0d_s0beit_sa_all.log");

      g_flLogAll = fopen(filename_all, "a");
      if(g_flLogAll == NULL) return;
   }

   GetLocalTime(&time);
   fprintf(g_flLogAll, "[%02d-%02d-%02d || %02d:%02d:%02d.%03d] ", time.wDay, time.wMonth, time.wYear,
	   time.wHour, time.wMinute, time.wSecond,time.wMilliseconds);
   va_start(ap, fmt);
   vfprintf(g_flLogAll, fmt, ap);
   va_end(ap);
   fprintf(g_flLogAll, "\n");
   fflush(g_flLogAll);
}

void LogChatbox(bool bLast, const char *fmt, ...)
{
	SYSTEMTIME time;
	va_list ap;
	
	if(g_flLogChatbox == NULL)
	{
		char filename[512];
		snprintf(filename, sizeof(filename), "%s\\%s", g_szWorkingDirectory, "m0d_s0beit_chatbox.log");
		
		remove(filename);
		g_flLogChatbox = fopen(filename, "a");
		if(g_flLogChatbox == NULL) return;
	}
	
	GetLocalTime(&time);
	fprintf(g_flLogChatbox, "[%02d:%02d:%02d.%02d] ", time.wHour, time.wMinute, time.wSecond,time.wMilliseconds);
	va_start(ap, fmt);
	vfprintf(g_flLogChatbox, fmt, ap);
	va_end(ap);
	fprintf(g_flLogChatbox, "\n");
	if(bLast) fprintf(g_flLogChatbox, "\n");
	fflush(g_flLogChatbox);

   if(g_flLogChatboxAll == NULL)
   {
      char filename_all[512];
	  snprintf(filename_all, sizeof(filename_all), "%s\\%s", g_szWorkingDirectory, "m0d_s0beit_chatbox_all.log");

      g_flLogChatboxAll = fopen(filename_all, "a");
      if(g_flLogChatboxAll == NULL) return;
   }

   GetLocalTime(&time);
   fprintf(g_flLogChatboxAll, "[%02d-%02d-%02d || %02d:%02d:%02d.%02d] ", time.wDay, time.wMonth, time.wYear,
	   time.wHour, time.wMinute, time.wSecond,time.wMilliseconds);
   va_start(ap, fmt);
   vfprintf(g_flLogChatboxAll, fmt, ap);
   va_end(ap);
   fprintf(g_flLogChatboxAll, "\n");
   if(bLast) fprintf(g_flLogChatboxAll, "\n");
   fflush(g_flLogChatboxAll);
}

void setDebugPointer(void *ptr)
{
   struct debug_info *debug = &cheat_state->debug;

   if(debug->hist_pos < DEBUG_HIST_LEN)
   {
      debug->hist_pos++;
   }
   else
   {
      memmove(debug->ptr, debug->ptr + 1, sizeof(debug->ptr) - sizeof(debug->ptr[0]));
      memmove(debug->offset, debug->offset + 1, sizeof(debug->offset) - sizeof(debug->offset[0]));
   }

   debug->ptr[debug->hist_pos] = (uint8_t *)ptr;
   debug->offset[debug->hist_pos] = 0;
   debug->data_prev_clear = 1;
}

#ifdef M0D_DEV
DWORD gtalog_log_eax_restore;
DWORD gtaload_log_continue = 0x53DED6;
DWORD file_loading_str;
char file_loading[356];
uint8_t _declspec(naked) gtaload_log(void)
{
	__asm
	{
		mov gtalog_log_eax_restore, eax
		lea eax, dword ptr [esp+0x8]
		mov eax, dword ptr [eax]
		test eax, eax
		je gtaload_log_out
		mov file_loading_str, eax
		pushad
	}
	memcpy_safe(file_loading, (const void *)file_loading_str, sizeof(file_loading));
	//Log("%s", file_loading);
	cheat_state_text("%s", file_loading);
	__asm popad

gtaload_log_out:;
	__asm
	{
		mov eax, gtalog_log_eax_restore
		mov eax, dword ptr [esp+0x4]
		test eax, eax
		jmp gtaload_log_continue
	}
}
#endif

static int init(void)
{
   traceLastFunc("init()");
   char filename[MAX_PATH];

	if(g_hOrigDll == NULL)
	{
		if(GetModuleFileName(g_hDllModule, g_szWorkingDirectory, sizeof(g_szWorkingDirectory) - 32) != 0)
		{
			if(strrchr(g_szWorkingDirectory, '\\') != NULL)
				*strrchr(g_szWorkingDirectory, '\\') = 0;
			else
				strcpy(g_szWorkingDirectory, ".");
			}
		else
		{
			strcpy(g_szWorkingDirectory, ".");
		}

		// Hello World
		Log("Initializing %s", NAME);

		// log windows version for idiots that forget to report it
		int osPlatform = (int)*(DWORD*)GTAvar_osPlatform;
		int osVer = (int)*(DWORD*)GTAvar_osVer;
		int winVer = (int)*(DWORD*)GTAvar_winVer;
		int winMajor = (int)*(DWORD*)GTAvar_winMajor;
		if (osPlatform == 2)
			Log("OS: Windows Version %d.%d.%d", winMajor, winVer, osVer);
		else
			Log("OS: Not Windows (%d.%d.%d)", winMajor, winVer, osVer);
		int D3D9UseVersion = (int)*(DWORD*)GTAvar__D3D9UseVersion;

		/*
		Log("D3D9UseVersion: %d", D3D9UseVersion);
		int DXVersion = (int)*(DWORD*)GTAvar__DXVersion;
		Log("DXVersion: %d", DXVersion);
		int windowsVersion = (int)*(DWORD*)GTAvar__windowsVersion;
		Log("windowsVersion: %d", windowsVersion);
		int CPUSupportsMMX = (int)*(DWORD*)__rwD3D9CPUSupportsMMX;
		int CPUSupports3DNow = (int)*(DWORD*)__rwD3D9CPUSupports3DNow;
		int CPUSupportsSSE = (int)*(DWORD*)__rwD3D9CPUSupportsSSE;
		int CPUSupportsSSE2 = (int)*(DWORD*)__rwD3D9CPUSupportsSSE2;
		if (!CPUSupportsMMX)
			Log("CPU Supports MMX: %d", CPUSupportsMMX);
		if (!CPUSupports3DNow)
			Log("CPU Supports 3DNow: %d", CPUSupports3DNow);
		if (!CPUSupportsSSE)
			Log("CPU Supports SSE: %d", CPUSupportsSSE);
		if (!CPUSupportsSSE2)
			Log("CPU Supports SSE2: %d", CPUSupportsSSE2);
		*/

#pragma warning(disable:4127)

		if(sizeof(struct vehicle_info) != 2584)
		{
			Log("sizeof(struct vehicle_info) == %d, aborting.", sizeof(struct vehicle_info));
			return 0;
		}

		if(sizeof(struct actor_info) != 1988)
		{
			Log("sizeof(struct actor_info) == %d, aborting.", sizeof(struct actor_info));
			return 0;
		}

#pragma warning(default:4127)

		ini_load();
		if(!set.i_have_edited_the_ini_file)
		{
			MessageBox(0,
			"Looks like you've not edited the .ini file like you were told to!\n"
			"\n"
			"Before you can use m0d_s0beit, you have to set \"i_have_edited_the_ini_file\" to true.\n",
			"You're a retard.", 0);
			ShellExecute(0, "open", "notepad", INI_FILE, g_szWorkingDirectory, SW_SHOW);
			return 0;
		}

		getSamp();
		InitScripting();

		GetSystemDirectory(filename, (UINT)(MAX_PATH - strlen("\\d3d9.dll") - 1));
		strlcat(filename, "\\d3d9.dll", sizeof(filename));
		g_hOrigDll = LoadLibrary(filename);
		if(g_hOrigDll == NULL)
		{
			Log("Failed to load %s", filename);
			return 0;
		}
		orig_Direct3DCreate9 = (D3DC9)GetProcAddress(g_hOrigDll, "Direct3DCreate9");
		if(orig_Direct3DCreate9 == NULL)
		{
			Log("%s does not export Direct3DCreate9!?", filename);
			FreeLibrary(g_hOrigDll);
			return 0;
		}

#ifdef M0D_DEV
		CDetour api;
		if(api.Create((uint8_t *)(uint32_t )0x53DED0, (uint8_t *)gtaload_log, DETOUR_TYPE_JMP, 5) == 0)
			Log("Failed to hook gtaload_log");
#endif

	}
	return 1;
}

IDirect3D9 *WINAPI sys_Direct3DCreate9(UINT SDKVersion)
{
	pDirect3D9 = NULL;
	if(init()) pDirect3D9 = new proxyIDirect3D9(orig_Direct3DCreate9(SDKVersion));
	//Log("Direct3DCreate9() called. Returning 0x%p.", pDirect3D9);
	return pDirect3D9;
}

LONG WINAPI unhandledExceptionFilter(struct _EXCEPTION_POINTERS *ExceptionInfo)
{
	Log(" ---------------------------------------------------------------------");
	Log(" %s has crashed.", NAME);

	Log(" Base address: 0x%p, SA:MP base address: 0x%p", g_hDllModule, g_dwSAMP_Addr);

	Log(" Exception at address: 0x%p, Last function processed: %s",
		ExceptionInfo->ExceptionRecord->ExceptionAddress, g_szLastFunc);

	int m_ExceptionCode = ExceptionInfo->ExceptionRecord->ExceptionCode;
	int m_exceptionInfo_0 = ExceptionInfo->ExceptionRecord->ExceptionInformation[0];
	int m_exceptionInfo_1 = ExceptionInfo->ExceptionRecord->ExceptionInformation[1];
	int m_exceptionInfo_2 = ExceptionInfo->ExceptionRecord->ExceptionInformation[2];
	switch (m_ExceptionCode)
	{
	case EXCEPTION_ACCESS_VIOLATION:
		Log(" Cause: EXCEPTION_ACCESS_VIOLATION");
		if (m_exceptionInfo_0 == 0)
		{
			// bad read
			Log(" Attempted to read from: 0x%08x", m_exceptionInfo_1);
		}
		else if (m_exceptionInfo_0 == 1)
		{
			// bad write
			Log(" Attempted to write to: 0x%08x", m_exceptionInfo_1);
		}
		else if (m_exceptionInfo_0 == 8)
		{
			// user-mode data execution prevention (DEP)
			Log(" Data Execution Prevention (DEP) at: 0x%08x", m_exceptionInfo_1);
		}
		else
		{
			// unknown, shouldn't happen
			Log(" Unknown access violation at: 0x%08x", m_exceptionInfo_1);
		}
		break;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		Log(" Cause: EXCEPTION_ARRAY_BOUNDS_EXCEEDED");
		break;
	case EXCEPTION_BREAKPOINT:
		Log(" Cause: EXCEPTION_BREAKPOINT");
		break;
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		Log(" Cause: EXCEPTION_DATATYPE_MISALIGNMENT");
		break;
	case EXCEPTION_FLT_DENORMAL_OPERAND:
		Log(" Cause: EXCEPTION_FLT_DENORMAL_OPERAND");
		break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		Log(" Cause: EXCEPTION_FLT_DIVIDE_BY_ZERO");
		break;
	case EXCEPTION_FLT_INEXACT_RESULT:
		Log(" Cause: EXCEPTION_FLT_INEXACT_RESULT");
		break;
	case EXCEPTION_FLT_INVALID_OPERATION:
		Log(" Cause: EXCEPTION_FLT_INVALID_OPERATION");
		break;
	case EXCEPTION_FLT_OVERFLOW:
		Log(" Cause: EXCEPTION_FLT_OVERFLOW");
		break;
	case EXCEPTION_FLT_STACK_CHECK:
		Log(" Cause: EXCEPTION_FLT_STACK_CHECK");
		break;
	case EXCEPTION_FLT_UNDERFLOW:
		Log(" Cause: EXCEPTION_FLT_UNDERFLOW");
		break;
	case EXCEPTION_ILLEGAL_INSTRUCTION:
		Log(" Cause: EXCEPTION_ILLEGAL_INSTRUCTION");
		break;
	case EXCEPTION_IN_PAGE_ERROR:
		Log(" Cause: EXCEPTION_IN_PAGE_ERROR");
		if (m_exceptionInfo_0 == 0)
		{
			// bad read
			Log(" Attempted to read from: 0x%08x", m_exceptionInfo_1);
		}
		else if (m_exceptionInfo_0 == 1)
		{
			// bad write
			Log(" Attempted to write to: 0x%08x", m_exceptionInfo_1);
		}
		else if (m_exceptionInfo_0 == 8)
		{
			// user-mode data execution prevention (DEP)
			Log(" Data Execution Prevention (DEP) at: 0x%08x", m_exceptionInfo_1);
		}
		else
		{
			// unknown, shouldn't happen
			Log(" Unknown access violation at: 0x%08x", m_exceptionInfo_1);
		}
		// log NTSTATUS
		Log(" NTSTATUS: 0x%08x", m_exceptionInfo_2);
		/*
		NT_SUCCESS(Status)
			Evaluates to TRUE if the return value specified by Status is a success type (0 - 0x3FFFFFFF) or an informational type (0x40000000 - 0x7FFFFFFF).
		NT_INFORMATION(Status)
			Evaluates to TRUE if the return value specified by Status is an informational type (0x40000000 - 0x7FFFFFFF).
		NT_WARNING(Status)
			Evaluates to TRUE if the return value specified by Status is a warning type (0x80000000 - 0xBFFFFFFF).
		NT_ERROR(Status)
			Evaluates to TRUE if the return value specified by Status is an error type (0xC0000000 - 0xFFFFFFFF)
		*/
		break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		Log(" Cause: EXCEPTION_INT_DIVIDE_BY_ZERO");
		break;
	case EXCEPTION_INT_OVERFLOW:
		Log(" Cause: EXCEPTION_INT_OVERFLOW");
		break;
	case EXCEPTION_INVALID_DISPOSITION:
		Log(" Cause: EXCEPTION_INVALID_DISPOSITION");
		break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		Log(" Cause: EXCEPTION_NONCONTINUABLE_EXCEPTION");
		break;
	case EXCEPTION_PRIV_INSTRUCTION:
		Log(" Cause: EXCEPTION_PRIV_INSTRUCTION");
		break;
	case EXCEPTION_SINGLE_STEP:
		Log(" Cause: EXCEPTION_SINGLE_STEP");
		break;
	case EXCEPTION_STACK_OVERFLOW:
		Log(" Cause: EXCEPTION_STACK_OVERFLOW");
		break;
	case DBG_CONTROL_C:
		Log(" Cause: DBG_CONTROL_C (WTF!)");
		break;
	default:
		Log(" Cause: %08x", m_ExceptionCode);
	}

	Log(" EAX: 0x%08x || ESI: 0x%08x", ExceptionInfo->ContextRecord->Eax, ExceptionInfo->ContextRecord->Esi);
	Log(" EBX: 0x%08x || EDI: 0x%08x", ExceptionInfo->ContextRecord->Ebx, ExceptionInfo->ContextRecord->Edi);
	Log(" ECX: 0x%08x || EBP: 0x%08x", ExceptionInfo->ContextRecord->Ecx, ExceptionInfo->ContextRecord->Ebp);
	Log(" EDX: 0x%08x || ESP: 0x%08x", ExceptionInfo->ContextRecord->Edx, ExceptionInfo->ContextRecord->Esp);

	Log(" ---------------------------------------------------------------------");

   return EXCEPTION_CONTINUE_SEARCH;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch(ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		g_hDllModule = hModule;
		SetUnhandledExceptionFilter(unhandledExceptionFilter);
		break;
	
	case DLL_PROCESS_DETACH:

		if(g_hOrigDll != NULL)
		{
			FreeLibrary(g_hOrigDll);
			g_hOrigDll = NULL;
			menu_free_all();
			ini_free();
			Log("Exited\n");
			if(set.chatbox_logging) LogChatbox(true, "Logging ended");
		}

		if(g_flLog != NULL)
		{
			fclose(g_flLog);
			g_flLog = NULL;
		}

		if(g_flLogAll != NULL)
		{
			fclose(g_flLogAll);
			g_flLogAll = NULL;
		}

		if(set.chatbox_logging)
		{
			if(g_flLogChatbox != NULL)
			{
				fclose(g_flLogChatbox);
				g_flLogChatbox = NULL;
			}
			if(g_flLogChatboxAll != NULL)
			{
				fclose(g_flLogChatboxAll);
				g_flLogChatboxAll = NULL;
			}
		}

		break;
	}
	
	return TRUE;
}
