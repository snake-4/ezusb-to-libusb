#define NOMINMAX
#include <Windows.h>
#include <vector>
#include <cstdint>
#include "EZUSB/ezusb.h"
#include <libusb-1.0/libusb.h>
#include "Globals.h"
#include "TranslationLayer.h"

using namespace EZUSB;

static constexpr uint16_t ANCHOR_DOWNLOAD_CHUNK_SIZE = 64;
static void TranslateIOCTL_Ezusb_VENDOR_REQUEST(LPVOID lpInBuffer, LPDWORD lpBytesReturned);
static bool TranslateIOCTL_EZUSB_ANCHOR_DOWNLOAD(PANCHOR_DOWNLOAD_CONTROL req, LPVOID lpOutBuffer, DWORD nOutBufferSize);
static void TranslateIOCTL_EZUSB_VENDOR_OR_CLASS_REQUEST(PVENDOR_OR_CLASS_REQUEST_CONTROL req, LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesReturned);

DWORD TL::TranslateIOTCL(HandleManager_t::handle_t handle, DWORD dwIOCTL, LPVOID lpInBuffer, DWORD nInBufferSize,
	LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesReturned)
{
	NTSTATUS status = ERROR_SUCCESS;

	switch (dwIOCTL) {
	case IOCTL_Ezusb_VENDOR_REQUEST: {
		TranslateIOCTL_Ezusb_VENDOR_REQUEST(lpInBuffer, lpBytesReturned);
		break;
	}
	case IOCTL_EZUSB_BULK_READ: {
		PBULK_TRANSFER_CONTROL bulkControl = (PBULK_TRANSFER_CONTROL)lpInBuffer;
		int bytesRead;

		int err = GUSBDev.value().ReadBulkTransfer(bulkControl->pipeNum, (unsigned char*)lpOutBuffer, nOutBufferSize, &bytesRead);
		if (err != LIBUSB_SUCCESS) {
			status = ERROR_GEN_FAILURE;
		}

		if (lpBytesReturned != NULL)
			*lpBytesReturned = bytesRead;
		break;
	}
	case IOCTL_EZUSB_BULK_WRITE: {
		PBULK_TRANSFER_CONTROL bulkControl = (PBULK_TRANSFER_CONTROL)lpInBuffer;
		int bytesWritten;

		int err = GUSBDev.value().WriteBulkTransfer(bulkControl->pipeNum, (unsigned char*)lpOutBuffer, nOutBufferSize, &bytesWritten);
		if (err != LIBUSB_SUCCESS) {
			status = ERROR_GEN_FAILURE;
		}

		if (lpBytesReturned != NULL)
			*lpBytesReturned = bytesWritten;
		break;
	}
	case IOCTL_Ezusb_RESETPIPE:
	case IOCTL_Ezusb_ABORTPIPE: {
		break;
	}
	case IOCTL_EZUSB_ANCHOR_DOWNLOAD: {
		if (nInBufferSize < sizeof(ANCHOR_DOWNLOAD_CONTROL)) {
			return ERROR_INVALID_PARAMETER;
		}

		PANCHOR_DOWNLOAD_CONTROL req = (PANCHOR_DOWNLOAD_CONTROL)lpInBuffer;
		if (!TranslateIOCTL_EZUSB_ANCHOR_DOWNLOAD(req, lpOutBuffer, nOutBufferSize)) {
			status = ERROR_GEN_FAILURE;
		}
		break;
	}
	case IOCTL_EZUSB_VENDOR_OR_CLASS_REQUEST: {
		if (nInBufferSize < sizeof(VENDOR_OR_CLASS_REQUEST_CONTROL)) {
			return ERROR_INVALID_PARAMETER;
		}

		PVENDOR_OR_CLASS_REQUEST_CONTROL req = (PVENDOR_OR_CLASS_REQUEST_CONTROL)lpInBuffer;
		TranslateIOCTL_EZUSB_VENDOR_OR_CLASS_REQUEST(req, lpOutBuffer, nOutBufferSize, lpBytesReturned);
		break;
	}
	case IOCTL_EZUSB_GET_DRIVER_VERSION: {
		if (nOutBufferSize < sizeof(EZUSB_DRIVER_VERSION)) {
			return ERROR_INVALID_PARAMETER;
		}

		PEZUSB_DRIVER_VERSION version = (PEZUSB_DRIVER_VERSION)lpOutBuffer;
		version->MajorVersion = EZUSB_MAJOR_VERSION;
		version->MinorVersion = EZUSB_MINOR_VERSION;
		version->BuildVersion = EZUSB_BUILD_VERSION;

		if (lpBytesReturned != NULL)
			*lpBytesReturned = sizeof(EZUSB_DRIVER_VERSION);

		break;
	}
	default:
		break;
	}

	return status;
}

static bool TranslateIOCTL_EZUSB_ANCHOR_DOWNLOAD(PANCHOR_DOWNLOAD_CONTROL req, LPVOID lpOutBuffer, DWORD nOutBufferSize) {
	//lpBytesReturned is undefined?

	uint8_t* ptr = (uint8_t*)lpOutBuffer;
	int chunkCount = ((nOutBufferSize + ANCHOR_DOWNLOAD_CHUNK_SIZE - 1) / ANCHOR_DOWNLOAD_CHUNK_SIZE);
	for (int i = 0; i < chunkCount; i++)
	{
		uint16_t wLength = ((i == (chunkCount - 1)) &&
			(nOutBufferSize % ANCHOR_DOWNLOAD_CHUNK_SIZE)) ?
			(nOutBufferSize % ANCHOR_DOWNLOAD_CHUNK_SIZE) : ANCHOR_DOWNLOAD_CHUNK_SIZE;

		uint8_t bRequest = ANCHOR_LOAD_INTERNAL;
		uint16_t wValue = (i * ANCHOR_DOWNLOAD_CHUNK_SIZE) + req->Offset;

		int transferred = GUSBDev.value().WriteControlTransfer(LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE,
			bRequest, wValue, 0, (unsigned char*)ptr, wLength);

		if (transferred != wLength) {
			return false;
		}
		ptr += ANCHOR_DOWNLOAD_CHUNK_SIZE;
	}

	return true;
}

static void TranslateIOCTL_EZUSB_VENDOR_OR_CLASS_REQUEST(PVENDOR_OR_CLASS_REQUEST_CONTROL req, LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesReturned)
{
	uint8_t requestType = 0;
	switch ((req->requestType << 2) | req->recepient)
	{
	case 0x04:
		requestType = LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_DEVICE;
		break;
	case 0x05:
		requestType = LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE;
		break;
	case 0x06:
		requestType = LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_ENDPOINT;
		break;
	case 0x07:
		requestType = LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_OTHER;
		break;
	case 0x08:
		requestType = LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE;
		break;
	case 0x09:
		requestType = LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_INTERFACE;
		break;
	case 0x0A:
		requestType = LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_ENDPOINT;
		break;
	case 0x0B:
		requestType = LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_OTHER;
		break;
	}

	int transferred = 0;
	if (req->direction) {
		transferred = GUSBDev.value().ReadControlTransfer(requestType,
			req->request, req->value, req->index, (unsigned char*)lpOutBuffer, nOutBufferSize);
	}
	else {
		transferred = GUSBDev.value().WriteControlTransfer(requestType,
			req->request, req->value, req->index, (unsigned char*)lpOutBuffer, nOutBufferSize);
	}

	if (lpBytesReturned != NULL)
		*lpBytesReturned = transferred;
}

static void TranslateIOCTL_Ezusb_VENDOR_REQUEST(LPVOID lpInBuffer, LPDWORD lpBytesReturned)
{
	PVENDOR_REQUEST_IN req = (PVENDOR_REQUEST_IN)lpInBuffer;
	int transferred = 0;
	if (req->direction) {
		transferred = GUSBDev.value().ReadControlTransfer(LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE,
			req->bRequest, req->wValue, req->wIndex, (unsigned char*)lpInBuffer, req->wLength); //nInBufferSize);
	}
	else {
		//lpBytesReturned is set to 0 on writes
		std::vector<uint8_t> outBuffer(req->wLength);
		if (req->wLength == 1)
		{
			outBuffer[0] = req->bData;
		}
		else
		{
			int i = 0;
			for (uint8_t& val : outBuffer) {
				val = i++;
			}
		}

		GUSBDev.value().WriteControlTransfer(LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE,
			req->bRequest, req->wValue, req->wIndex, (unsigned char*)outBuffer.data(), req->wLength);
	}

	if (lpBytesReturned != NULL)
		*lpBytesReturned = transferred;
}
