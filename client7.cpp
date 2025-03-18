#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

using namespace std;

void sendMessage(SOCKET& sock, const string& message) {
    send(sock, message.c_str(), message.size(), 0);
}

string receiveMessage(SOCKET& sock) {
    char buffer[DEFAULT_BUFLEN] = { 0 };
    int result = recv(sock, buffer, DEFAULT_BUFLEN, 0);
    return string(buffer, result);
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    struct addrinfo hints = {}, * result;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    getaddrinfo("localhost", DEFAULT_PORT, &hints, &result);

    SOCKET ConnectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    connect(ConnectSocket, result->ai_addr, (int)result->ai_addrlen);
    freeaddrinfo(result);

    int choice;
    string login, password;

    while (true) {
        cout << "Menu:\n1. Login\n2. Register\n3. Exit\nYour choice: ";
        cin >> choice;
        cin.ignore();

        if (choice == 3) break;

        cout << "Username: "; getline(cin, login);
        cout << "Password: "; getline(cin, password);

        string request = (choice == 1 ? "LOGIN " : "REGISTER ") + login + " " + password;
        sendMessage(ConnectSocket, request);

        string response = receiveMessage(ConnectSocket);
        cout << "Server: " << response << endl;
    }

    closesocket(ConnectSocket);
    WSACleanup();
    return 0;
}
