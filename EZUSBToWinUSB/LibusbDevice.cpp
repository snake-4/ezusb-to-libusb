#include <iostream>
#include <memory>
#include <libusb-1.0/libusb.h>
#include "Globals.h"
#include "LibusbDevice.h"

LIBUSBDevice::InternalHandle::~InternalHandle()
{
	libusb_close(val);
}

LIBUSBDevice::InternalHandle::InternalHandle(libusb_device_handle* val)
	: val(val) { }

LIBUSBDevice::LIBUSBDevice(int vid, int pid, int timeout_ms, int configIdx, int interfaceIdx, int altsetting)
	: vid(vid), pid(pid), configIdx(configIdx), interfaceIdx(interfaceIdx), interfaceDesc(nullptr), timeout_ms(timeout_ms)
{
	auto newHandle = libusb_open_device_with_vid_pid(GLibUsbCtx, vid, pid);
	if (newHandle == nullptr) {
		return;
	}

	handle = std::make_shared<InternalHandle>(newHandle);
	auto dev = libusb_get_device(handle->val);

	libusb_config_descriptor* config;
	libusb_get_config_descriptor(dev, configIdx, &config);

	auto interface = &config->interface[interfaceIdx];
	libusb_claim_interface(handle->val, interfaceIdx);

	interfaceDesc = &interface->altsetting[altsetting];
}

int LIBUSBDevice::WriteControlTransfer(uint8_t request_type, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
	const void* buffer, uint16_t bufferLength)
{
	if (handle == nullptr) {
		return 0;
	}

	return libusb_control_transfer(handle->val, request_type | LIBUSB_ENDPOINT_OUT, bRequest, wValue, wIndex, (unsigned char*)buffer, bufferLength, timeout_ms);
}

int LIBUSBDevice::ReadControlTransfer(uint8_t request_type, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
	const void* buffer, uint16_t bufferLength)
{
	if (handle == nullptr) {
		return 0;
	}

	return libusb_control_transfer(handle->val, request_type | LIBUSB_ENDPOINT_IN, bRequest, wValue, wIndex, (unsigned char*)buffer, bufferLength, timeout_ms);
}

int LIBUSBDevice::ReadBulkTransfer(int pipeNum, void* buffer, int bufferLength, int* bytesRead) {
	if (handle == nullptr) {
		return LIBUSB_ERROR_NO_DEVICE;
	}

	int endpointAddress = GetEndpointAddressByPipeNum(pipeNum);
	return libusb_bulk_transfer(handle->val, endpointAddress,
		(unsigned char*)buffer, bufferLength, bytesRead, timeout_ms);
}

int LIBUSBDevice::WriteBulkTransfer(int pipeNum, const void* buffer, int bufferLength, int* bytesWritten) {
	if (handle == nullptr) {
		return LIBUSB_ERROR_NO_DEVICE;
	}

	int endpointAddress = GetEndpointAddressByPipeNum(pipeNum);
	return libusb_bulk_transfer(handle->val, endpointAddress,
		(unsigned char*)(buffer), bufferLength, bytesWritten, timeout_ms);
}

int LIBUSBDevice::GetEndpointAddressByPipeNum(int pipeNum) {
	if (pipeNum >= interfaceDesc->bNumEndpoints) {
		return -1;
	}

	return interfaceDesc->endpoint[pipeNum].bEndpointAddress;
}
