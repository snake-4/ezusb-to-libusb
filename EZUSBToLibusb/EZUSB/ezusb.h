#pragma once
#include <minwindef.h>

namespace EZUSB {
	constexpr int EZUSB_MAJOR_VERSION = 01;
	constexpr int EZUSB_MINOR_VERSION = 01;
	constexpr int EZUSB_BUILD_VERSION = 0000;

	//The ezusb.sys only opens the first config, first interface and the first alternate setting
	constexpr int USB_CONFIG_INDEX = 0;
	constexpr int USB_INTERFACE_INDEX = 0;
	constexpr int USB_INTERFACE_ALTERNATE_SETTING = 0;
#include "ezusbsys_internal.h"
}
