#pragma once
#include <string>

struct Configuration_t {
	Configuration_t(int usbVID, int usbPID, int usbConfigIndex, int usbTimeoutMS, int debug, const std::string& ezusbDriverPath);

	const int USBVID;
	const int USBPID;
	const int USBConfigIndex;
	const int USBTimeoutMS;

	const int Debug;
	const std::string EZUSBDriverPath;

	static Configuration_t LoadFromIniFile(const std::string& iniFileName);
};
