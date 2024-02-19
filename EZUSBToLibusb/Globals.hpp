#pragma once
#include <optional>
#include "LibusbDevice.hpp"
#include "HandleManager.hpp"
#include "Config.hpp"

extern GHandleManager_t GHandleManager;
extern libusb_context* GLibUsbCtx;
extern std::optional<LIBUSBDevice> GUSBDev;
extern Configuration_t GConfig;
extern HANDLE GUSBInitThread;
