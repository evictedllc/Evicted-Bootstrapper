#include <windows.h>
#include <WinSock2.h>
#include <iphlpapi.h>
#include <stdio.h>

#pragma comment(lib, "iphlpapi.lib")

int main()
{
    // Disable IP logging by disabling IPv6
    DWORD dwValue = 0;
    DWORD dwSize = sizeof(dwValue);
    if (ERROR_SUCCESS == RegGetValue(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Services\\Tcpip6\\Parameters"), TEXT("DisabledComponents"), RRF_RT_REG_DWORD, NULL, &dwValue, &dwSize))
    {
        dwValue |= 0x20;
        RegSetValueEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Services\\Tcpip6\\Parameters"), NULL, REG_DWORD, (BYTE*)&dwValue, sizeof(dwValue));
    }

    // Clear ARP cache to remove any stored IP address
    FlushIpNetTable(AF_UNSPEC);

    // Disable DHCP to prevent IP logging from DHCP server
    SERVICE_STATUS status;
    SC_HANDLE hServiceMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    SC_HANDLE hService = OpenService(hServiceMgr, TEXT("Dhcp"), SERVICE_ALL_ACCESS);
    ControlService(hService, SERVICE_CONTROL_STOP, &status);
    ChangeServiceConfig(hService, SERVICE_NO_CHANGE, SERVICE_DISABLED, SERVICE_NO_CHANGE, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    CloseServiceHandle(hService);
    CloseServiceHandle(hServiceMgr);

    // Clear DNS cache to remove any stored IP address
    system("ipconfig /flushdns");

    // Disable Windows event logging to prevent IP logging from the operating system
    system("wevtutil cl Application");
    system("wevtutil cl Security");
    system("wevtutil cl System");

    return 0;
}
