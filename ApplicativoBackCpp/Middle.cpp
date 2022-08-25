using namespace std;

#define _WIN32_WINNT 0x501

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <string>
#include <cpr/cpr.h>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "5000"


int __cdecl main(int argc, char** argv)
{
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL,
        * ptr = NULL,
        hints;

    const char* address = "192.168.2.99";

    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int n;
    int recvbuflen = DEFAULT_BUFLEN;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    iResult = getaddrinfo(address, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    do {
        std::cout << "Action: Retrieve all commands" << std::endl;
        auto r = cpr::Get(cpr::Url{ "http://localhost:8080/api/commands" });

        json data = nlohmann::json::parse(r.text);

        for (auto& elm : data.items())
        {
            nlohmann::json object = elm.value();

            string cmd_s = object["cmd"];
            string len_s = object["len"];
            string pos_s = object["pos"];
            string id_cmd_s = object["id"];
            bool done = true;
            string user_s = object["user"];
            string start_s = "0x02";
            string finish_s = "0x03";

            double cmd = std::stod(cmd_s);
            double pos = std::stod(pos_s);
            double start = std::stod(start_s);
            double finish = std::stod(finish_s);
            double len = cmd+ pos+ start+ finish;

            uint8_t sendbuf[] = {0x02,pos,cmd,0x03,len};

            int bytes_to_send = sizeof(sendbuf);
            int bytes_sent = 0;

            json my_json;
            my_json["id"] = id_cmd_s;
            my_json["cmd"]= cmd_s;
            my_json["done"]= done;
            my_json["len"]= len_s;
            my_json["pos"]= pos_s;
            my_json["response"]= "";
            my_json["user"]= user_s;

            do
            {
                n = iResult = send(ConnectSocket, (char*)sendbuf + bytes_sent, bytes_to_send - bytes_sent, 0);
                if (n < 0)
                {
                    printf("send failed with error: %d\n", WSAGetLastError());
                    closesocket(ConnectSocket);
                    WSACleanup();
                    return 1;
                }
                bytes_sent += n;
                std::cout << "Action: Update Command with Id = "+ id_cmd_s << std::endl;
                auto p = cpr::Put(cpr::Url{ "http://localhost:8080/api/command/" + id_cmd_s },
                    cpr::Body{ my_json.dump() },
                    cpr::Header{ { "Content-Type", "application/json" } });
                std::cout << "Returned Status:" << p.status_code << std::endl;
            } while (bytes_sent < bytes_to_send);
        }

        Sleep(2000);
    } while (1);

    //Nel caso ricevere i dati quando facciamo richiesta cmd=0x30 e per chiudere la socket usare questo codice

    // shutdown the connection since no more data will be sent

    /*iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    // Receive until the peer closes the connection
    do {

        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0)
            printf("Bytes received: %d\n", iResult);
        else if (iResult == 0)
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());

    } while (iResult > 0);

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;*/
}
