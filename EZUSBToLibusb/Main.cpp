#include <Windows.h>
#include <detours/detours.h>
#include <libusb-1.0/libusb.h>
#include <optional>
#include <utility>
#include "Hooks.h"
#include "Globals.h"
#include "Config.h"
#include "Utils.h"
#include "EZUSB/ezusb.h"

GHandleManager_t GHandleManager;
libusb_context* GLibUsbCtx = nullptr;
std::optional<LIBUSBDevice> GUSBDev;
Configuration_t GConfig = Configuration_t::LoadFromJsonFile("eu2lu_config.json");

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

		for (const auto& entry : GConfig.USBSearchList)
		{
			auto dev = LIBUSBDevice::OpenDevice(entry.VID, entry.PID, GConfig.USBTimeoutMS,
				EZUSB::USB_CONFIG_INDEX, EZUSB::USB_INTERFACE_INDEX, EZUSB::USB_INTERFACE_ALTERNATE_SETTING);

			if (dev.has_value()) {
				GUSBDev = dev;
				break;
			}
		}
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetachHooks();
		DetourTransactionCommit();

		//libusb_exit must be called after all USB objects are destructured
		//libusb_exit(GLibUsbCtx);
	}
	return TRUE;
}
