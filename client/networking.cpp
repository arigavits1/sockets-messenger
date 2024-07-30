#include "include/networking.h"
#include <functional>

struct Data
{
    char buffer[256];
    char name[11];
} typedef Data;

void sendToServer(int sockfd, std::string& message)
{
    Data sendData;
    strncpy(sendData.buffer, message.c_str(), sizeof(sendData.buffer));
    strncpy(sendData.name, name.c_str(), sizeof(sendData.name));
    char buffer[sizeof(sendData)];
    memcpy(buffer, &sendData, sizeof(buffer));
    ssize_t bytes_sent = send(sockfd, buffer, sizeof(buffer), 0);
    if (bytes_sent < 0)
    {
        perror("Error sending data");
    }
    if (strcmp(sendData.buffer, "bye") == 0)
    {
        exit(0);
    }
    return;
}

int networkSetup()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("Socket error");
        return -1;
    }

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &(server_address.sin_addr));
    if (connect(sockfd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0)
    {
        std::cerr << "Error connecting to the server" << std::endl;
        close(sockfd);
        exit(1);
    }
    std::cout << "Waiting to connect..." << std::endl;
    send(sockfd, name.c_str(), strlen(name.c_str()) * sizeof(char), 0);

    char buffer[4];
    ssize_t bytes_received = recv(sockfd, buffer, sizeof(buffer), 0);
    if (bytes_received < 0)
    {
        std::cerr << "Error receiving confirmation code" << std::endl;
        exit(-1);
    }
    int code = atoi(buffer);
    if (code == 201)
    {
        std::cerr << "Username already taken" << std::endl;
        exit(-1);
    }

    std::cout << "Connected to server!\n";
    return sockfd;
}

void* recvFromServer(int sockfd, std::string* imguiBuffer, std::atomic<bool>& should_run)
{
    fd_set readfds;
    struct timeval tv;
    Data recvData;
    char buffer[sizeof(recvData)];

    for (;;)
    {
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);

        tv.tv_sec = 0;
        tv.tv_usec = 10000;

        int select_result = select(sockfd + 1, &readfds, NULL, NULL, &tv);

        if (select_result == -1)
        {
            perror("Select error");
            break;
        }
        else if (select_result == 0)
        {
            continue;
        }

        memset(buffer, 0, sizeof(buffer));
        ssize_t bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        memcpy(&recvData, buffer, sizeof(recvData));
        recvData.buffer[strlen(recvData.buffer)] = '\0';

        if (bytes_received < 0)
        {
            perror("Error receiving data");
            break;
        }
        else if (bytes_received == 0)
        {
            continue;;
        }

        if (strcmp(recvData.buffer, "") == 0)
        {
            continue;
        }

        std::string textTempBuffer = recvData.buffer;
        std::string nameTempBuffer = recvData.name;
        *imguiBuffer += "\n\n" + nameTempBuffer + ": " + textTempBuffer;
        std::cout << recvData.buffer << "\n" << std::flush;

        if (strcmp(recvData.buffer, "bye") == 0)
        {
            std::cout << "Disconnecting..." << std::endl;
            should_run = false;
            break;
        }
    }

    return NULL;
}
