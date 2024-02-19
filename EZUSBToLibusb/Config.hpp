#pragma once
#include <string_view>
#include <string>
#include <vector>
#include <optional>

struct Configuration_t {
	struct USBSearchListEntry {
		const int VID;
		const int PID;
		const std::optional<std::string> SerialNumber;

		USBSearchListEntry(int VID, int PID, std::string_view SerialNumber);
		USBSearchListEntry(int VID, int PID);
	};

	Configuration_t();
	Configuration_t(int USBTimeoutMS, int Debug, std::string_view EZUSBDriverPath, std::vector<USBSearchListEntry> USBSearchList);

	const std::vector<USBSearchListEntry> USBSearchList;
	const int USBTimeoutMS;

	const int Debug;
	const std::string EZUSBDriverPath;

	static Configuration_t LoadFromJsonFile(const std::string& jsonFileName);
};
