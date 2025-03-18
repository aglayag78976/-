#include <iostream>
#include <string>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

using namespace std;

struct User {
    string login;
    string password;
};

vector<User> users;

void sendMessage(SOCKET& sock, const string& message) {
    send(sock, message.c_str(), message.size(), 0);
}

string receiveMessage(SOCKET& sock) {
    char buffer[DEFAULT_BUFLEN] = { 0 };
    int result = recv(sock, buffer, DEFAULT_BUFLEN, 0);
    return string(buffer, result);
}

bool registerUser(const string& login, const string& password) {
    for (const auto& user : users) {
        if (user.login == login) return false;
    }
    users.push_back({ login, password });
    return true;
}

bool loginUser(const string& login, const string& password) {
    for (const auto& user : users) {
        if (user.login == login && user.password == password) return true;
    }
    return false;
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    struct addrinfo hints = {}, * result;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);

    SOCKET ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    listen(ListenSocket, SOMAXCONN);
    freeaddrinfo(result);

    cout << "Server started...\n";

    while (true) {
        SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
        string request = receiveMessage(ClientSocket);

        size_t space1 = request.find(' ');
        size_t space2 = request.find(' ', space1 + 1);
        string command = request.substr(0, space1);
        string login = request.substr(space1 + 1, space2 - space1 - 1);
        string password = request.substr(space2 + 1);

        string response;
        if (command == "REGISTER") {
            response = registerUser(login, password) ? "Registration successful" : "Username already exists";
        }
        else if (command == "LOGIN") {
            response = loginUser(login, password) ? "Login successful" : "Invalid username or password";
        }

        sendMessage(ClientSocket, response);
        closesocket(ClientSocket);
    }

    closesocket(ListenSocket);
    WSACleanup();
    return 0;
}
