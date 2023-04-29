#include <windows.h>

int main()
{
    // Get the process ID of the program to limit
    DWORD pid = GetCurrentProcessId();

    // Get the number of processors on the system
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    DWORD numProcessors = sysinfo.dwNumberOfProcessors;

    // Calculate the affinity mask to limit the program to using a single processor
    DWORD_PTR affinityMask = 0;
    for (DWORD i = 0; i < numProcessors; i++)
    {
        affinityMask |= (DWORD_PTR)1 << i;
    }

    // Set the process affinity mask to limit the program to using a single processor
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (hProcess != NULL)
    {
        SetProcessAffinityMask(hProcess, affinityMask);
        CloseHandle(hProcess);
    }

    // Run the program that needs to be limited in CPU usage
    // ...

    return 0;
}
