#include <windows.h>
#include <tlhelp32.h>

int main()
{
    DWORD pid = GetCurrentProcessId();
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe32 = { sizeof(PROCESSENTRY32) };
    Process32First(snapshot, &pe32);
    do {
        if (pe32.th32ProcessID == pid) {
            break;
        }
    } while (Process32Next(snapshot, &pe32));
    CloseHandle(snapshot);

    HANDLE handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    TCHAR path[MAX_PATH];
    if (GetModuleFileNameEx(handle, 0, path, MAX_PATH) == 0) {
        printf("Error: %d\n", GetLastError());
        return 1;
    }
    CloseHandle(handle);

    TCHAR newpath[MAX_PATH];
    GetTempFileName(TEXT("."), TEXT("hiding"), 0, newpath);
    MoveFileEx(path, newpath, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);

    return 0;
}
