#ifdef SUPERPRO_WINMM_BUILD
#define MMNOSOUND
#include <Windows.h>

using pSndPlaySoundA_t = BOOL(WINAPI*)(LPCSTR, UINT);
pSndPlaySoundA_t pSndPlaySoundA = nullptr;

extern "C" {
	__declspec(dllexport) BOOL sndPlaySoundA(LPCSTR pszSound, UINT fuSound) {
		if (pSndPlaySoundA == nullptr) {
			HMODULE winmm = LoadLibrary(TEXT("winmm"));
			if (winmm != NULL) {
				FARPROC proc = GetProcAddress(winmm, "sndPlaySoundA");
				if (proc != NULL) {
					pSndPlaySoundA = reinterpret_cast<pSndPlaySoundA_t>(proc);
				}
			}
		}
		if (pSndPlaySoundA != nullptr) {
			return pSndPlaySoundA(pszSound, fuSound);
		}
		return FALSE;
	}
}
#endif
