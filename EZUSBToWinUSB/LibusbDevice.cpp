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


static libusb_device_handle* OpenDeviceByVIDPID(uint16_t vid, uint16_t pid)
{
	libusb_device** deviceList;
	int count = libusb_get_device_list(GLibUsbCtx, &deviceList);
	if (count < 0) {
		return nullptr;
	}

	for (int i = 0; i < count; i++) {
		libusb_device* device = deviceList[i];
		struct libusb_device_descriptor desc;
		if (libusb_get_device_descriptor(device, &desc) == LIBUSB_SUCCESS) {
			if (desc.idVendor == vid && desc.idProduct == pid) {
				libusb_device_handle* handle = nullptr;
				int status = libusb_open(device, &handle);
				if (status != LIBUSB_SUCCESS) {
					std::cout << "libusb_open() error: " << libusb_strerror(status) << '\n';
				}
				libusb_free_device_list(deviceList, 1);
				return handle;
			}
		}
	}

	libusb_free_device_list(deviceList, 1);
	return nullptr;
}

LIBUSBDevice::LIBUSBDevice(int vid, int pid, int timeout_ms, int configIdx, int interfaceIdx, int altsetting)
	: vid(vid), pid(pid), configIdx(configIdx), interfaceIdx(interfaceIdx), interfaceDesc(nullptr), timeout_ms(timeout_ms)
{
	auto newHandle = OpenDeviceByVIDPID(vid, pid);
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
		return LIBUSB_ERROR_NO_DEVICE;
	}

	return libusb_control_transfer(handle->val, request_type | LIBUSB_ENDPOINT_OUT, bRequest, wValue, wIndex, (unsigned char*)buffer, bufferLength, timeout_ms);
}

int LIBUSBDevice::ReadControlTransfer(uint8_t request_type, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
	const void* buffer, uint16_t bufferLength)
{
	if (handle == nullptr) {
		return LIBUSB_ERROR_NO_DEVICE;
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
