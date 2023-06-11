#include <Windows.h>
#include <detours/detours.h>
#include <libusb-1.0/libusb.h>
#include <optional>
#include "Hooks.h"
#include "Globals.h"
#include "EZUSB/ezusb.h"

HandleManager_t GHandleManager;
libusb_context* GLibUsbCtx = nullptr;
std::optional<LIBUSBDevice> GUSBDev;

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved)
{
	UNREFERENCED_PARAMETER(hinst);
	UNREFERENCED_PARAMETER(reserved);

	if (DetourIsHelperProcess())
	{
		return TRUE;
	}

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DetourRestoreAfterWith();

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		AttachHooks();
		DetourTransactionCommit();

		libusb_init(&GLibUsbCtx);
		GUSBDev.emplace(LIBUSBDevice(0x6022, 0x0001, 10000, 0, EZUSB::USB_INTERFACE_INDEX, EZUSB::USB_INTERFACE_ALTERNATE_SETTING));
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetachHooks();
		DetourTransactionCommit();

		libusb_exit(GLibUsbCtx);
	}
	return TRUE;
}
