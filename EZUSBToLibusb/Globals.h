#pragma once
#include <optional>
#include "LibusbDevice.h"
#include "HandleManager.h"
#include "Config.h"

extern GHandleManager_t GHandleManager;
extern libusb_context* GLibUsbCtx;
extern std::optional<LIBUSBDevice> GUSBDev;
extern Configuration_t GConfig;
extern HANDLE GUSBInitThread;
