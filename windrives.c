#include "windrives.h"
#include <wchar.h>
#include <wctype.h>

static VOID FillDriveInfo(LPDRIVEINFO lpDst, LPCWSTR lpszDrivePath)
{
    wcsncpy_s(lpDst->cLetter, 4, lpszDrivePath, 3);

    ULARGE_INTEGER uliFreeBytesAvailable, uliTotalBytes, uliTotalFreeBytes;
    GetDiskFreeSpaceExW(lpszDrivePath, &uliFreeBytesAvailable, &uliTotalBytes, &uliTotalFreeBytes);

    lpDst->dTotalGb = (double)uliTotalBytes.QuadPart / 0x40000000;
    lpDst->dFreeGb  = (double)uliTotalFreeBytes.QuadPart / 0x40000000;

    GetVolumeInformationW(
        lpszDrivePath,
        lpDst->szLabel,
        MAX_PATH,
        &lpDst->dwSerialNumber,
        &lpDst->dwMaxComponentLen,
        &lpDst->dwFileSystemFlags,
        lpDst->szFileSystem,
        MAX_PATH
    );

    lpDst->uType = GetDriveTypeW(lpszDrivePath);
}

static UINT GetDrivesImpl(LPDRIVEINFO lpDst, UINT uCount, BOOL bRemovable)
{
    if (!lpDst || uCount == 0 || uCount > 25) return 0;

    DWORD dwDrives = GetLogicalDrives();
    UINT uDrivesFound = 0;

    for (UINT i = 0; i < 26 && i < uCount; ++i) {
        if (!(dwDrives & (1 << i))) continue;

        WCHAR szPath[] = { L'A' + i, L':', L'\\', L'\0' };
        UINT uType = GetDriveTypeW(szPath);
        if ((bRemovable && uType != DRIVE_REMOVABLE) || uType == DRIVE_UNKNOWN || uType == DRIVE_NO_ROOT_DIR) continue;

        FillDriveInfo(&lpDst[uDrivesFound++], szPath);
    }

    return uDrivesFound;
}

UINT GetDrives(LPDRIVEINFO lpDst, UINT uCount)
{
    return GetDrivesImpl(lpDst, uCount, FALSE);
}

UINT GetRemovableDrives(LPDRIVEINFO lpDst, UINT uCount)
{
    return GetDrivesImpl(lpDst, uCount, TRUE);
}

UINT GetDrivesCount()
{
    UINT count = 0;
    DWORD dwDrives = GetLogicalDrives();

    for (UINT i = 0; i < 26; ++i)
        if (dwDrives & (1 << i)) count++;

    return count;
}

void WaitForNewDriveConnected(DWORD dwTimeoutMs)
{
    if (dwTimeoutMs == 0) return;

    DWORD dwCurDrives = GetLogicalDrives();

    while (dwTimeoutMs > 0) {
        if (GetLogicalDrives() != dwCurDrives) break;
        DWORD dwSleep = dwTimeoutMs < 10 ? dwTimeoutMs : 10;
        Sleep(dwSleep);
        dwTimeoutMs -= dwSleep;
    }
}

BOOL GetDriveByLetter(WCHAR letter, LPDRIVEINFO lpDst)
{
    if (!lpDst) return FALSE;
    WCHAR szPath[] = { towupper(letter), L':', L'\\', L'\0' };
    if (!(GetLogicalDrives() & (1 << (towupper(letter) - L'A')))) return FALSE;
    FillDriveInfo(lpDst, szPath);
    return TRUE;
}
