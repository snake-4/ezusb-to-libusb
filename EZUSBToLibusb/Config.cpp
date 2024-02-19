#include <Windows.h>
#include <string>
#include <string_view>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include "Config.hpp"

static long JSONGetNumber(const nlohmann::json& json, long defaultValue);

Configuration_t::USBSearchListEntry::USBSearchListEntry(int VID, int PID)
	: VID(VID), PID(PID)
{
}

Configuration_t::USBSearchListEntry::USBSearchListEntry(int VID, int PID, std::string_view SerialNumber)
	: VID(VID), PID(PID), SerialNumber(SerialNumber)
{
}

Configuration_t::Configuration_t()
	: USBTimeoutMS(0), Debug(0)
{
}

Configuration_t::Configuration_t(int USBTimeoutMS, int Debug, std::string_view EZUSBDriverPath, std::vector<USBSearchListEntry> USBSearchList)
	: USBTimeoutMS(USBTimeoutMS), Debug(Debug), EZUSBDriverPath(EZUSBDriverPath), USBSearchList(USBSearchList)
{
}

Configuration_t Configuration_t::LoadFromJsonFile(const std::string& jsonFileName)
{
	std::ifstream file(jsonFileName);
	nlohmann::json jsonData;
	file >> jsonData;

	int usbTimeoutMS = 10000;
	if (jsonData.contains("USBTimeoutMS"))
		usbTimeoutMS = JSONGetNumber(jsonData["USBTimeoutMS"], usbTimeoutMS);

	int debug = 0;
	if (jsonData.contains("Debug"))
		debug = JSONGetNumber(jsonData["Debug"], debug);

	std::string ezusbDriverPath = "\\\\.\\Ezusb-0";
	if (jsonData.contains("EZUSBDriverPath") && jsonData["EZUSBDriverPath"].is_string())
		ezusbDriverPath = jsonData["EZUSBDriverPath"];

	std::vector<Configuration_t::USBSearchListEntry> searchList;

	try {
		for (const auto& entry : jsonData["USBSearchList"])
		{
			int vid = JSONGetNumber(entry["VID"], 0);
			int pid = JSONGetNumber(entry["PID"], 0);

			if (entry.contains("SerialNumber")
				&& entry["SerialNumber"].is_string()
				&& !entry["SerialNumber"].get<std::string>().empty())
				searchList.emplace_back(vid, pid, entry["SerialNumber"]);
			else
				searchList.emplace_back(vid, pid);
		}
	}
	catch (std::exception& exc) {
		std::cout << "Error: Caught an exception while parsing the config \"" << exc.what() << "\"\n";
	}

	if (searchList.empty()) {
		std::cout << "Error: USBSearchList is empty.\n";
	}

	return Configuration_t(usbTimeoutMS, debug, ezusbDriverPath, searchList);
}

long JSONGetNumber(const nlohmann::json& json, long defaultValue)
{
	if (json.is_number()) {
		return json.get<long>();
	}
	if (json.is_string()) {
		auto str = json.get<std::string>();
		try {
			//Try hex
			return std::stol(str, nullptr, 16);
		}
		catch (std::exception&) {}

		try {
			//Try decimal
			return std::stol(str, nullptr, 10);
		}
		catch (std::exception&) {}
	}

	return defaultValue;
}
