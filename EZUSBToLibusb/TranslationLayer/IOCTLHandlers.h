#pragma once
#include <Windows.h>
#include "TranslationLayer.h"

namespace TL {
	//Lowercase functions
	ioctl_hdl_return_t Hdl_IOCTL_Ezusb_GET_CURRENT_CONFIG(LPVOID, DWORD, LPVOID, DWORD);
	ioctl_hdl_return_t Hdl_IOCTL_Ezusb_GET_STRING_DESCRIPTOR(LPVOID, DWORD, LPVOID, DWORD);
	ioctl_hdl_return_t Hdl_IOCTL_Ezusb_VENDOR_REQUEST(LPVOID, DWORD, LPVOID, DWORD);
	ioctl_hdl_return_t Hdl_IOCTL_Ezusb_ANCHOR_DOWNLOAD(LPVOID, DWORD, LPVOID, DWORD);
	ioctl_hdl_return_t Hdl_IOCTL_Ezusb_RESET(LPVOID, DWORD, LPVOID, DWORD);
	ioctl_hdl_return_t Hdl_IOCTL_Ezusb_RESETPIPE(LPVOID, DWORD, LPVOID, DWORD);
	//Uppercase functions
	ioctl_hdl_return_t Hdl_IOCTL_EZUSB_ANCHOR_DOWNLOAD(LPVOID, DWORD, LPVOID, DWORD);
	ioctl_hdl_return_t Hdl_IOCTL_EZUSB_BULK_READ(LPVOID, DWORD, LPVOID, DWORD);
	ioctl_hdl_return_t Hdl_IOCTL_EZUSB_BULK_WRITE(LPVOID, DWORD, LPVOID, DWORD);
	ioctl_hdl_return_t Hdl_IOCTL_EZUSB_VENDOR_OR_CLASS_REQUEST(LPVOID, DWORD, LPVOID, DWORD);
	ioctl_hdl_return_t Hdl_IOCTL_EZUSB_GET_DRIVER_VERSION(LPVOID, DWORD, LPVOID, DWORD);
	ioctl_hdl_return_t Hdl_IOCTL_EZUSB_SET_FEATURE(LPVOID, DWORD, LPVOID, DWORD);
}
