#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return 1;
    }

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) {
        return 1;
    }

    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(4444);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sock, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        return 1;
    }

    char buffer[1024];
    int recv_len;
    SOCKADDR_IN client_addr;
    int client_addr_len = sizeof(client_addr);
    while ((recv_len = recvfrom(sock, buffer, sizeof(buffer), 0, (SOCKADDR*)&client_addr, &client_addr_len)) != SOCKET_ERROR) {
        printf("Received packet from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
