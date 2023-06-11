#define MMNOSOUND
#include <Windows.h>

extern "C" {
	__declspec(dllexport) BOOL sndPlaySoundA() {
		return TRUE;
	}
}