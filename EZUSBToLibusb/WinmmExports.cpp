#ifdef SUPERPRO_WINMM_BUILD
#define MMNOSOUND
#include <Windows.h>

extern "C" {
	__declspec(dllexport) BOOL sndPlaySoundA() {
		return TRUE;
	}
}
#endif
