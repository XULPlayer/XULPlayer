#include <shlobj.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*pfnDropInCallback)(DROPFILES *dropfiles);

void RegisterDropWindow(HWND hwnd, pfnDropInCallback callback);
void UnregisterDropWindow(HWND hwnd);

#ifdef __cplusplus
}
#endif
