#pragma once
#include <optional>
#include "LibusbDevice.h"
#include "HandleManager.h"
#include "Config.h"

using HandleManager_t = HandleManager<std::nullptr_t>;

extern HandleManager_t GHandleManager;
extern libusb_context* GLibUsbCtx;
extern std::optional<LIBUSBDevice> GUSBDev;
extern Configuration_t GConfig;
