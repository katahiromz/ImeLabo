#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <vector>
#include "MString.hpp"
#include "util.h"
#include "convert.h"

#pragma comment(lib, "ws2_32.lib")

static const char *s_default_port = DEFAULT_PORT;

struct CWinSock
{
    BOOL m_bInit;
    WSADATA m_wsaData;
    INT m_iResult;

    CWinSock()
    {
        m_bInit = FALSE;
        ZeroMemory(&m_wsaData, sizeof(m_wsaData));
        m_iResult = 0;
    }
    ~CWinSock()
    {
        Close();
    }

    BOOL Init(WORD wVersionRequested)
    {
        m_iResult = ::WSAStartup(wVersionRequested, &m_wsaData);
        if (m_iResult == 0)
        {
            m_bInit = TRUE;
            return TRUE;
        }
        PrintError();
        return FALSE;
    }
    void Close()
    {
        if (m_bInit)
        {
            ::WSACleanup();
            m_bInit = FALSE;
        }
    }

    INT Error() const
    {
        return m_iResult;
    }

    void PrintError()
    {
        if (m_iResult)
            printf("m_iResult: %d\n", m_iResult);
    }
};

struct CAddrInfo
{
    INT m_iResult = 0;
    struct addrinfo m_hints, *m_result;
    CAddrInfo()
    {
        m_iResult = 0;
        m_result = NULL;
    }
    ~CAddrInfo()
    {
        FreeAddrInfo();
    }
    void FreeAddrInfo()
    {
        if (m_result)
        {
            ::freeaddrinfo(m_result);
            m_result = NULL;
        }
    }
    struct addrinfo *operator->() const
    {
        return m_result;
    }
    BOOL GetAddrInfo()
    {
        ZeroMemory(&m_hints, sizeof(m_hints));
        m_hints.ai_family = AF_INET;
        m_hints.ai_socktype = SOCK_STREAM;
        m_hints.ai_protocol = IPPROTO_TCP;
        m_hints.ai_flags = AI_PASSIVE;
        m_iResult = ::getaddrinfo(NULL, s_default_port, &m_hints, &m_result);
        if (m_iResult != 0)
        {
            printf("getaddrinfo failed: %d\n", m_iResult);
            return FALSE;
        }
        return TRUE;
    }
};

struct CSocket
{
    SOCKET m_socket;
    CSocket()
    {
        m_socket = INVALID_SOCKET;
    }
    ~CSocket()
    {
        Close();
    }
    void Close()
    {
        if (m_socket != INVALID_SOCKET)
        {
            closesocket(m_socket);
            printf("Closed socket: %d\n", m_socket);
            m_socket = INVALID_SOCKET;
        }
    }
    operator SOCKET() const
    {
        return m_socket;
    }
    BOOL Create(CAddrInfo& addrInfo)
    {
        m_socket = ::socket(addrInfo->ai_family, addrInfo->ai_socktype, addrInfo->ai_protocol);
        if (m_socket == INVALID_SOCKET)
        {
            printf("Error at socket(): %ld\n", WSAGetLastError());
            return FALSE;
        }
        printf("Socket created: %d\n", m_socket);
        return TRUE;
    }
    BOOL Bind(const struct sockaddr *name, INT namelen)
    {
        INT iResult = ::bind(m_socket, name, namelen);
        if (iResult != 0)
        {
            printf("bind failed with error: %d\n", WSAGetLastError());
            return FALSE;
        }
        return TRUE;
    }
    BOOL Accept(SOCKET socket, struct sockaddr *addr, INT *addrlen)
    {
        m_socket = ::accept(socket, addr, addrlen);
        if (m_socket == INVALID_SOCKET)
        {
            printf("accept failed: %d\n", WSAGetLastError());
            return FALSE;
        }
        printf("Socket accepted: %d\n", m_socket);
        return TRUE;
    }
    INT Recv(char *buf, INT len, INT flags)
    {
        INT bytes = ::recv(m_socket, buf, len, flags);
        if (bytes > 0)
        {
            printf("Bytes received: %d\n", bytes);
            return bytes;
        }

        if (!bytes)
        {
            printf("Connection closing...\n");
            return bytes;
        }

        printf("recv failed: %d\n", WSAGetLastError());
        return bytes;
    }
    INT Send(const char *buf, INT len, INT flags)
    {
        INT bytes = ::send(m_socket, buf, len, flags);
        if (bytes == SOCKET_ERROR)
        {
            printf("send failed: %d\n", WSAGetLastError());
            return 0;
        }
        printf("Bytes sent: %d\n", bytes);
        return bytes;
    }
    BOOL Listen(INT backlog)
    {
        INT iResult = ::listen(m_socket, backlog);
        if (iResult == SOCKET_ERROR)
        {
            printf("Listen failed with error: %ld\n", WSAGetLastError());
            return FALSE;
        }
        return TRUE;
    }
};

std::string
JustMakeResult(const std::wstring& wide)
{
    std::string utf8;

#if 0
    utf8 = MZ_Utf8FromWide(wide.c_str());
    return utf8;
#endif

    if (!(wide[0] == L'/' && wide[1] == L'?'))
        return "error";

    auto q = wide.substr(2);
    std::vector<std::wstring> params;
    mstr_split(params, q, L"&");
    printf("params: %d\n", (INT)params.size());

    std::wstring result;
    for (auto& param : params)
    {
        printf("param: '%s'\n", MZ_AnsiFromWide(param.c_str()).c_str());

        std::vector<std::wstring> pair;
        mstr_split(pair, param, L"=");
        if (pair.size() != 2)
            continue;

        if (pair[0] == L"langpair")
            continue;

        if (pair[0] == L"text")
        {
            result = MZ_ImeConvert(pair[1]);
            break;
        }

        puts("error");
    }

    printf("result: '%s'\n", MZ_AnsiFromWide(result.c_str()).c_str());

    utf8 = MZ_Utf8FromWide(result.c_str());
    return utf8;
}

INT DoHttpResponse(CSocket& ListenSocket, CSocket& ClientSocket, const char *input_str, INT input_bytes)
{
    std::string input(input_str, input_bytes);
    mstr_replace_all(input, "\r\n", "\n");

    std::vector<std::string> lines;
    mstr_split(lines, input, "\n");

    if (lines.empty())
        return 0;

    std::vector<std::string> fields;
    mstr_split(fields, lines[0], " ");
    if (fields.size() != 3 || fields[0] != "GET")
        return 0;

    std::string response;
#if 1
    std::string decoded(MZ_UrlDecode(fields[1].c_str()));
    if (decoded.empty())
        return 0;

    std::wstring wide = MZ_WideFromUtf8(decoded.c_str());
    std::string ansi = MZ_AnsiFromWide(wide.c_str());
    printf("ansi: %s\n", ansi.c_str());

    std::string result = JustMakeResult(wide);

    response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain; charset=utf-8\r\n";
    response += std::string("Content-Length: ") + std::to_string(result.size()) + "\r\n";
    response += "\r\n";
    response += result;
#else
    response = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\n\r\n";
    response += "<html><body>";
    for (auto& line : lines)
    {
        response += line;
        response += "<br />";
    }
    response += "</body></html>";
#endif

    return ClientSocket.Send(response.c_str(), (INT)response.size(), 0);
}

void usage(void)
{
    puts("Usage: server.exe [port]");
}

int main(int argc, char **argv)
{
    if (argc >= 2)
    {
        if (isdigit((BYTE)argv[1][0]))
        {
            s_default_port = argv[1];
        }
        else
        {
            usage();
            return 0;
        }
    }

    CMZDictSet mzDicts;
    if (!mzDicts.Load())
        return -1;

    // Initialize Winsock
    CWinSock winSock;
    if (!winSock.Init(MAKEWORD(2, 2)))
        return 1;

    // Get address info
    CAddrInfo addrInfo;
    if (!addrInfo.GetAddrInfo())
        return 2;

    // Create socket
    CSocket ListenSocket;
    if (!ListenSocket.Create(addrInfo))
        return 3;

    // Bind
    if (!ListenSocket.Bind(addrInfo->ai_addr, (int)addrInfo->ai_addrlen))
        return 4;

    // Listen
    if (!ListenSocket.Listen(SOMAXCONN))
        return 5;

    printf("HTTP Server is running on port %s...\n", s_default_port);
    printf("Press Ctrl+C to quit...\n");

    char recvbuf[DEFAULT_BUFLEN];
    for (;;)
    {
        // Accept
        CSocket ClientSocket;
        if (!ClientSocket.Accept(ListenSocket, NULL, NULL))
        {
            ClientSocket.Close();
            continue;
        }

        // Recieve
        INT bytes = ClientSocket.Recv(recvbuf, sizeof(recvbuf), 0);
        if (bytes > 0)
        {
            // Response
            DoHttpResponse(ListenSocket, ClientSocket, recvbuf, bytes);
        }

        ClientSocket.Close();
    }

    return 0;
}
