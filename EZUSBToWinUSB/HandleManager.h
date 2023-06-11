#pragma once
#include <cstdint>
#include <Windows.h>
#include <unordered_map>
#include "LibusbDevice.h"

template <typename T>
class HandleManager {
public:
	using handle_t = ::HANDLE;

	void RegisterHandle(handle_t hdl, const T& val = T()) {
		handles.emplace(hdl, val);
	}
	void UnregisterHandle(handle_t hdl) {
		handles.erase(hdl);
	}
	bool IsHandleValid(handle_t hdl) {
		return handles.find(hdl) != handles.end();
	}
	T& GetHandleObject(handle_t hdl) {
		return handles.at(hdl);
	}

private:
	std::unordered_map<handle_t, T> handles;
};


