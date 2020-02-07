// crashfirefox.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include <stdio.h>
#include <TlHelp32.h>

int wmain(int argc, wchar_t* argv[])
{
    const wchar_t* appname = L"firefox.exe";
    switch (argc) {
    case 1:
        break;
    case 2:
        appname = argv[1];
        break;
    default:
        fwprintf(stderr, L"Usage: crashfirefox.exe [PID|processname.exe]\n");
        return 1;
    }

    DWORD pid = 0;
    if (appname[0] >= '0' && appname[0] <= '9') {
        pid = _wtoi(appname);
        if (0 == pid) {
            fwprintf(stderr, L"Unable to convert PID %s to an integer.\n", appname);
            return 1;
        }
    }
    if (0 == pid) {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (INVALID_HANDLE_VALUE == snapshot) {
            fwprintf(stderr, L"Couldn't enumerate processes: %d\n", GetLastError());
            return 1;
        }
        PROCESSENTRY32 pe = {
            sizeof(pe)
        };
        for (BOOL ok = Process32First(snapshot, &pe); ok; ok = Process32Next(snapshot, &pe)) {
            fwprintf(stderr, L"Checking process %d, \"%s\"\n", pe.th32ProcessID, pe.szExeFile);
            if (0 == wcscmp(pe.szExeFile, appname)) {
                pid = pe.th32ProcessID;
                break;
            }
        }
        if (0 == pid) {
            fwprintf(stderr, L"Couldn't not find process \"%s\".\n", appname);
            return 1;
        }
    }

    HANDLE targetProc = OpenProcess(PROCESS_VM_OPERATION | PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION,
        FALSE, pid);
    if (NULL == targetProc) {
        fwprintf(stderr, L"Couldn't open target process with PID %d\n", pid);
        return 1;
    }

    // Create a thread that starts at DebugBreak and should immediately crash
    auto debugBreak = GetProcAddress(GetModuleHandle(L"kernel32.dll"), "DebugBreak");
    HANDLE hThread = CreateRemoteThread(targetProc, NULL, 0, (LPTHREAD_START_ROUTINE)debugBreak, 0, 0, 0);
    if (NULL == hThread) {
        fwprintf(stderr, L"Could not create remote thread. Error: %X\n", GetLastError());
        CloseHandle(targetProc);
        return 1;
    }
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    CloseHandle(targetProc);

    return 0;
}

