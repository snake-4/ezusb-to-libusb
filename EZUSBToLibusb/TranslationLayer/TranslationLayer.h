#pragma once
#include <Windows.h>
#include <optional>
#include <functional>
#include "../HandleManager.h"

namespace TL {
	DWORD TranslateIOTCL(GHandleManager_t::handle_t handle, DWORD dwIOCTL, LPVOID lpInBuffer, DWORD nInBufferSize,
		LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesReturned);

	struct ioctl_hdl_return_t {
		ioctl_hdl_return_t(DWORD _error);
		ioctl_hdl_return_t(DWORD _error, DWORD _bytesReturned);
		const DWORD error;
		const std::optional<DWORD> bytesReturned;
	};
	using ioctl_handler_t = std::function<ioctl_hdl_return_t(LPVOID, DWORD, LPVOID, DWORD)>;
}
