#include <iostream>
#include <memory>
#include <optional>
#include <libusb-1.0/libusb.h>
#include "Globals.h"
#include "LibusbDevice.h"

static libusb_device_handle* OpenDeviceByVIDPID(uint16_t vid, uint16_t pid);

LIBUSBDevice::InternalHandle::~InternalHandle()
{
	libusb_close(val);
}

LIBUSBDevice::InternalHandle::InternalHandle(libusb_device_handle* val)
	: val(val) { }

std::optional<LIBUSBDevice> LIBUSBDevice::OpenDevice(int vid, int pid, int timeout_ms, int configIdx, int interfaceIdx, int altsetting)
{
	auto newHandle = OpenDeviceByVIDPID(vid, pid);
	if (newHandle == nullptr) {
		return {};
	}

	return LIBUSBDevice(newHandle, vid, pid, timeout_ms, configIdx, interfaceIdx, altsetting);
}

LIBUSBDevice::LIBUSBDevice(libusb_device_handle* _handle, int vid, int pid, int timeout_ms, int configIdx, int interfaceIdx, int altsetting)
	: vid(vid), pid(pid), timeout_ms(timeout_ms), configIdx(configIdx), interfaceIdx(interfaceIdx), altsetting(altsetting)
{
	handle = std::make_shared<InternalHandle>(_handle);
	auto dev = libusb_get_device(handle->val);

	libusb_config_descriptor* config;
	libusb_get_config_descriptor(dev, configIdx, &config);

	auto iface = &config->interface[interfaceIdx];
	libusb_claim_interface(handle->val, interfaceIdx);

	interfaceDesc = const_cast<libusb_interface_descriptor*>(&iface->altsetting[altsetting]);
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

std::vector<uint8_t> LIBUSBDevice::GetStringDescriptor(uint8_t desc_index, uint16_t langid)
{
	std::vector<uint8_t> vec(255, 0); //Apparently some devices fail on size > 255

	int status = libusb_get_string_descriptor(handle->val, desc_index, langid, (unsigned char*)vec.data(), vec.size());

	if (status >= 0) {
		vec.resize(status);
	}

	return vec;
}

bool LIBUSBDevice::ReopenDevice() {
	auto newDev = LIBUSBDevice::OpenDevice(vid, pid, timeout_ms, configIdx, interfaceIdx, altsetting);
	if (!newDev.has_value()) {
		return false;
	}
	*this = newDev.value();
	return true;
}

int LIBUSBDevice::ResetDevice() {
	if (handle == nullptr) {
		return LIBUSB_ERROR_NO_DEVICE;
	}

	int status = libusb_reset_device(handle->val);
	if (status == LIBUSB_ERROR_NOT_FOUND) {
		handle = nullptr;
		if (ReopenDevice()) {
			status = LIBUSB_SUCCESS;
		}
	}

	return status;
}

int LIBUSBDevice::GetEndpointAddressByPipeNum(int pipeNum) {
	if (pipeNum >= interfaceDesc->bNumEndpoints) {
		return -1;
	}

	return interfaceDesc->endpoint[pipeNum].bEndpointAddress;
}

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
