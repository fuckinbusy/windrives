#ifndef WIN32_I_HATE_WINAPI
#define WIN32_I_HATE_WINAPI

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdbool.h>

typedef struct tagDRIVEINFO {
    WCHAR  cLetter[4];
    WCHAR  szLabel[MAX_PATH];
    WCHAR  szFileSystem[MAX_PATH];
    DWORD  uType;
    DWORD  dwSerialNumber;
    DWORD  dwMaxComponentLen;
    DWORD  dwFileSystemFlags;
    double dTotalGb;
    double dFreeGb;
} DRIVEINFO, *LPDRIVEINFO;

#ifdef __cplusplus
extern "C" {
#endif

UINT GetDrives(LPDRIVEINFO lpDst, UINT uCount);
UINT GetRemovableDrives(LPDRIVEINFO lpDst, UINT uCount);
UINT GetDrivesCount();
void WaitForNewDriveConnected(DWORD dwTimeoutMs);
BOOL GetDriveByLetter(WCHAR letter, LPDRIVEINFO lpDst);

#ifdef __cplusplus
}
#endif

#endif // WIN32_I_HATE_WINAPI
