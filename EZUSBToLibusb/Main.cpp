#include <Windows.h>
#include <detours/detours.h>
#include <libusb-1.0/libusb.h>
#include <optional>
#include "Hooks.h"
#include "Globals.h"
#include "Config.h"
#include "Utils.h"
#include "EZUSB/ezusb.h"

HandleManager_t GHandleManager;
libusb_context* GLibUsbCtx = nullptr;
std::optional<LIBUSBDevice> GUSBDev;
Configuration_t GConfig = Configuration_t::LoadFromIniFile("eu2lu_config.ini");

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

		if (GConfig.Debug > 0) {
			AllocConsole();
			BindCrtHandlesToStdHandles(true, true, true);
			_putenv("LIBUSB_DEBUG=4");
		}

		libusb_init(&GLibUsbCtx);
		GUSBDev.emplace(
			GConfig.USBVID, GConfig.USBPID, GConfig.USBTimeoutMS, GConfig.USBConfigIndex,
			EZUSB::USB_INTERFACE_INDEX, EZUSB::USB_INTERFACE_ALTERNATE_SETTING
		);
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
