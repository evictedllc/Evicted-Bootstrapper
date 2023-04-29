#pragma once

#include <WinSock2.h>
#include <ws2tcpip.h>

namespace evicted::AntiProxy
{
    bool IsProxyConnected()
    {
        SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        if (sock == INVALID_SOCKET)
        {
            return false;
        }

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(80);
        inet_pton(AF_INET, "216.58.194.174", &addr.sin_addr);

        if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
        {
            closesocket(sock);
            return true;
        }

        closesocket(sock);
        return false;
    }
}
