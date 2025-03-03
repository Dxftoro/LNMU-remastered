#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <WinBase.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <stdio.h>

#pragma warning(disable: 4996)
#define MAX_CLIENT_COUNT 4
#define MAX_MSG_SIZE	 4096
#define MAX_NAME_SIZE	 32

using std::cout;
using std::cerr;
using std::endl;
using std::cin;

std::mutex locker;
std::string serverChatName = "Server";

#include "server_util.h"
#include "comhandler.h"

struct Packet {
	char name[MAX_NAME_SIZE];
	char msg[MAX_MSG_SIZE];

	Packet() {}

	Packet(const char* name, const char* msg) {
		strcpy(this->name, name);
		strcpy(this->msg, msg);
	}

	Packet(std::string name, std::string msg) {
		strcpy(this->name, name.c_str());
		strcpy(this->msg, msg.c_str());
	}
};

#include "server_commands.h"

bool configurate(std::string& hostname, int& port, size_t& serverSize, std::string& sign) {
	std::ifstream fin("serverinfo.txt", std::ios::binary);

	if (!fin) return false;

	fin >> hostname >> port >> serverSize;

	std::string tmp;
	while (!fin.eof() && std::getline(fin, tmp)) {
		sign += tmp + '\n';
	}

	fin.close();
	return true;
}

void tryTellTo(ClientManager* clientManager, const std::string& msg, size_t author) {
	cout << "Running tell" << endl;
	std::string name;
	
	size_t msgStart = 0;
	for (size_t i = 1; i < msg.size(); i++) {
		if (msg[i] == ' ') {
			msgStart = i + 1;
			break;
		}
		else name += msg[i];
	}

	size_t target = clientManager->findByName(name);
	if (target == SIZE_MAX) {
		std::string warnMsg = "User not found!";
		Packet packet(serverChatName, warnMsg);
		clientManager->sendTo(author, (char*)&packet, sizeof(packet), NULL);
		return;
	}

	std::string msgText = "";
	for (size_t i = msgStart; i < msg.size(); i++) {
		msgText += msg[i];
	}

	Packet packet(clientManager->getConnection(author).name, msgText);
	clientManager->sendTo(target, (char*)&packet, sizeof(packet), NULL);
}

void handleClient(ClientManager* clientManager, SOCKET client, size_t index,
					CommandHandler* cmd) {

	while (true) {
		Packet packet;
		if (recv(client, (char*)&packet, sizeof(packet), NULL) > 0) {
			if (strlen(packet.msg) > 0) {
				if (packet.msg[0] == '/') {
					cmd->handle(packet.msg, index);
				}
				else clientManager->sendAll((char*)&packet, sizeof(packet), NULL, index);
			}
		}
		else {
			std::string disconnMsg = clientManager->getConnection(index).name + " disconnected.";
			clientManager->removeConnection(index);

			strcpy(packet.name, serverChatName.c_str());
			strcpy(packet.msg, disconnMsg.c_str());

			cout << disconnMsg << endl;
			clientManager->sendAll((char*)&packet, sizeof(packet), NULL);

			return;
		}
	}
}

int main() {
	std::string hostname, sign = "";
	int port = 27015;
	size_t serverSize = MAX_CLIENT_COUNT;

	if (!configurate(hostname, port, serverSize, sign)) {
		cerr << "Can't find info file!" << endl;
		return -2;
	}

	ClientManager clientManager(serverSize);
	CommandHandler cmd(&clientManager);
	std::vector<std::thread*> connectionThreads;

	cmd.setDefault(cmdDef);
	cmd.add("/tell", cmdTell);
	cmd.add("/ls", cmdLs);
	cmd.add("/help", cmdHelp);

	WSAData wsalib_d;
	WORD DllVersion = MAKEWORD(1, 3);

	if (WSAStartup(DllVersion, &wsalib_d) != 0) {
		cerr << "WinSocket startup error!" << endl;
		return -1;
	}

	SOCKADDR_IN addr;
	addr.sin_addr.s_addr = inet_addr(hostname.c_str());
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	size_t addrSize = sizeof(addr);

	SOCKET listeningSocket = socket(AF_INET, SOCK_STREAM, NULL);
	bind(listeningSocket, (SOCKADDR*)&addr, addrSize);
	listen(listeningSocket, SOMAXCONN);

	cout << "Server listening on " + hostname + ":" + std::to_string(port) << endl;
	cout << "Server size: " << serverSize << endl;

	SOCKET clientConnection;

	while (true) {
		int addrSizeInt = (int)addrSize;
		clientConnection = accept(listeningSocket, (SOCKADDR*)&addr, &addrSizeInt);

		if (!clientConnection) {
			cerr << "Something went wrong when client tried to connect..." << endl;
		}
		else {
			char clientName[MAX_NAME_SIZE];

			recv(clientConnection, clientName, MAX_NAME_SIZE, NULL);
			size_t index = clientManager.addConnection(clientName, clientConnection);

			Packet helloPacket;
			strcpy(helloPacket.name, serverChatName.c_str());

			if (index == SIZE_MAX) {
				strcpy(helloPacket.msg, "Connection aborted: server is full!");
				send(clientConnection, (char*)&helloPacket, sizeof(helloPacket), NULL);
			}
			else {
				strcpy(helloPacket.msg, sign.c_str());
				send(clientConnection, (char*)&helloPacket, sizeof(helloPacket), NULL);

				std::thread* clientHandler = new std::thread(handleClient, 
					&clientManager, clientConnection, index, &cmd);

				connectionThreads.push_back(clientHandler);

				std::string connectionMsg = std::string(clientName) + " connected!";

				Packet connAnnouncement(serverChatName, connectionMsg);
				cout << connectionMsg << endl;
				clientManager.sendAll((char*)&connAnnouncement, sizeof(Packet), NULL, index);
			}
		}
	}

	closesocket(listeningSocket);

	for (size_t i = 0; i < connectionThreads.size(); i++) {
		if (connectionThreads[i] != nullptr) {
			connectionThreads[i]->detach();
			delete connectionThreads[i];
		}
	}

	system("pause");
	return 0;
}