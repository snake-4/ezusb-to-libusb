#include <Windows.h>
#include <cstring>
#include <detours/detours.h>
#include "Hooks.h"
#include "TranslationLayer.h"
#include "Globals.h"

HANDLE(WINAPI* origCreateFileA)(LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE) = CreateFileA;

HANDLE WINAPI hkCreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile)
{
	if (strcmp(lpFileName, "\\\\.\\Ezusb-0") != 0 &&
		strcmp(lpFileName, "\\\\.\\Xeltekusb-0") != 0)
	{
		return origCreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes,
			dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	}

	HANDLE reservedHandle;
	HANDLE currentProc = GetCurrentProcess();
	DuplicateHandle(currentProc, INVALID_HANDLE_VALUE, currentProc, &reservedHandle, 0, FALSE, DUPLICATE_SAME_ACCESS);

	GHandleManager.RegisterHandle(reservedHandle);

	SetLastError(ERROR_SUCCESS);
	return reservedHandle;
}

BOOL(WINAPI* origDeviceIoControl)(HANDLE, DWORD, LPVOID, DWORD, LPVOID, DWORD, LPDWORD, LPOVERLAPPED) = DeviceIoControl;

BOOL WINAPI hkDeviceIoControl(HANDLE hDevice, DWORD dwIoControlCode, LPVOID lpInBuffer,
	DWORD nInBufferSize, LPVOID lpOutBuffer, DWORD nOutBufferSize,
	LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped)
{
	if (!GHandleManager.IsHandleValid(hDevice)) {
		return origDeviceIoControl(hDevice, dwIoControlCode, lpInBuffer, nInBufferSize, lpOutBuffer,
			nOutBufferSize, lpBytesReturned, lpOverlapped);
	}

	DWORD err = TL::TranslateIOTCL(hDevice, dwIoControlCode, lpInBuffer, nInBufferSize, lpOutBuffer,
		nOutBufferSize, lpBytesReturned);
	SetLastError(err);

	return err == ERROR_SUCCESS;
}

BOOL(WINAPI* origCloseHandle)(HANDLE) = CloseHandle;

BOOL WINAPI hkCloseHandle(HANDLE hObject)
{
	GHandleManager.UnregisterHandle(hObject);
	return origCloseHandle(hObject);
}

void AttachHooks() {
	DetourAttach(&(PVOID&)origCreateFileA, hkCreateFileA);
	DetourAttach(&(PVOID&)origDeviceIoControl, hkDeviceIoControl);
	DetourAttach(&(PVOID&)origCloseHandle, hkCloseHandle);
}

void DetachHooks() {
	DetourDetach(&(PVOID&)origCreateFileA, hkCreateFileA);
	DetourDetach(&(PVOID&)origDeviceIoControl, hkDeviceIoControl);
	DetourDetach(&(PVOID&)origCloseHandle, hkCloseHandle);
}
