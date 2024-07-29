#pragma once
#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <atomic>

#define PORT 8080
extern std::string name;

struct Data;
int networkSetup();
void* sendToServer(int sockfd, std::string& message);
void* recvFromServer(int sockfd, std::string* imguiBuffer, std::atomic<bool>& should_run);
