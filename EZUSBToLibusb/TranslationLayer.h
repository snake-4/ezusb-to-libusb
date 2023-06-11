#pragma once
#include <Windows.h>
#include "Globals.h"

namespace TL {
	DWORD TranslateIOTCL(HandleManager_t::handle_t handle, DWORD dwIOCTL, LPVOID lpInBuffer, DWORD nInBufferSize,
		LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesReturned);
}
