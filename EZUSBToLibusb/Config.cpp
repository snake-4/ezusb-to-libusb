#include <Windows.h>
#include <string>
#include "Config.h"

//codecvt was deprecated ;(
static std::string ConvertWideToANSI(const std::wstring& wstr);
static std::wstring ConvertAnsiToWide(const std::string& str);

Configuration_t::Configuration_t(int usbVID, int usbPID, int usbConfigIndex, int usbTimeoutMS, int debug, const std::string& ezusbDriverPath)
	: USBVID(usbVID), USBPID(usbPID), USBConfigIndex(usbConfigIndex), USBTimeoutMS(usbTimeoutMS), Debug(debug), EZUSBDriverPath(ezusbDriverPath) { }

Configuration_t Configuration_t::LoadFromIniFile(const std::string& iniFileName)
{
	wchar_t modulePath[MAX_PATH];
	GetModuleFileNameW(nullptr, modulePath, MAX_PATH);
	std::wstring moduleDir = modulePath;
	moduleDir = moduleDir.substr(0, moduleDir.find_last_of(L'\\'));
	std::wstring iniFilePath = moduleDir + L"\\" + ConvertAnsiToWide(iniFileName);

	int usbVID = static_cast<int>(GetPrivateProfileIntW(L"Configuration", L"USBVID", 0, iniFilePath.c_str()));
	int usbPID = static_cast<int>(GetPrivateProfileIntW(L"Configuration", L"USBPID", 0, iniFilePath.c_str()));
	int usbConfigIndex = static_cast<int>(GetPrivateProfileIntW(L"Configuration", L"USBConfigIndex", 0, iniFilePath.c_str()));
	int usbTimeoutMS = static_cast<int>(GetPrivateProfileIntW(L"Configuration", L"USBTimeoutMS", 10000, iniFilePath.c_str()));

	wchar_t ezusbDriverPath[MAX_PATH];
	GetPrivateProfileStringW(L"Configuration", L"EZUSBDriverPath", L"\\\\.\\Ezusb-0", ezusbDriverPath, MAX_PATH, iniFilePath.c_str());
	int debug = static_cast<int>(GetPrivateProfileIntW(L"Configuration", L"Debug", 0, iniFilePath.c_str()));

	return Configuration_t(usbVID, usbPID, usbConfigIndex, usbTimeoutMS, debug, ConvertWideToANSI(ezusbDriverPath));
}

static std::string ConvertWideToANSI(const std::wstring& wstr)
{
	int count = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.length(), NULL, 0, NULL, NULL);
	std::string str(count, 0);
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, &str[0], count, NULL, NULL);
	return str;
}

static std::wstring ConvertAnsiToWide(const std::string& str)
{
	int count = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), NULL, 0);
	std::wstring wstr(count, 0);
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), &wstr[0], count);
	return wstr;
}