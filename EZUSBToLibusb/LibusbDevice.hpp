#pragma once
#include <Windows.h>
#include <cstdint>
#include <memory>
#include <optional>
#include <vector>
#include <libusb-1.0/libusb.h>

class LIBUSBDevice {
public:
	static std::optional<LIBUSBDevice> OpenDevice(int vid, int pid, int timeout_ms = 10000, int configIdx = 0, int interfaceIdx = 0, int altsetting = 0);

	int WriteControlTransfer(uint8_t request_type, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
		const void* buffer, uint16_t bufferLength);
	int ReadControlTransfer(uint8_t request_type, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
		const void* buffer, uint16_t bufferLength);

	int ReadBulkTransfer(int pipeNum, void* buffer, int bufferLength, int* bytesRead);
	int WriteBulkTransfer(int pipeNum, const void* buffer, int bufferLength, int* bytesWritten);

	int ResetDevice();
	int ResetEndpoint(int pipeNum);
	std::vector<uint8_t> GetStringDescriptor(uint8_t desc_index, uint16_t langid);

private:
	LIBUSBDevice(libusb_device_handle* handle, int vid, int pid, int timeout_ms, int configIdx, int interfaceIdx, int altsetting);

	int GetEndpointAddressByPipeNum(int pipeNum);
	bool ReopenDevice();

	class InternalHandle {
	public:
		InternalHandle(libusb_device_handle*);
		~InternalHandle();
		libusb_device_handle* val;
	};

	std::shared_ptr<InternalHandle> handle;
	libusb_interface_descriptor* interfaceDesc;
	int vid, pid, configIdx, interfaceIdx, altsetting;
	unsigned int timeout_ms;
};
