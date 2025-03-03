#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <WinBase.h>
#include <iostream>
#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <stdio.h>

#pragma warning(disable: 4996)
#define MAX_MSG_SIZE	 4096
#define MAX_NAME_SIZE	 32

using std::cout;
using std::cerr;
using std::endl;
using std::cin;

struct Packet {
	char name[MAX_NAME_SIZE];
	char msg[MAX_MSG_SIZE];
};

struct Address {
	std::string ip;
	int port = 0;
};

Address makeAddress(std::string strAddress) {
	std::string ip = "", portStr = "";
	size_t ddotPos = 0;

	for (size_t i = 0; i < strAddress.size(); i++) {
		if (strAddress[i] == ':') {
			ddotPos = i;
			break;
		}
		else ip += strAddress[i];
	}

	for (size_t i = ddotPos + 1; i < strAddress.size(); i++) {
		portStr += strAddress[i];
	}
	int port = std::stoi(portStr);

	return Address{ ip, port };
}

void handleClient(SOCKET* serverSocket) {
	char msg[MAX_MSG_SIZE];

	while (true) {
		Packet packet;
		if (recv(*serverSocket, (char*)&packet, sizeof(packet), NULL) > 0) {
			printf("[%s] %s\n", packet.name, packet.msg);
		}
		else {
			closesocket(*serverSocket);
			cout << "Connection dropped by server!" << endl;
			break;
		}
	}
}

int main() {
	WSAData wsalib_d;
	WORD DllVersion = MAKEWORD(1, 3);

	if (WSAStartup(DllVersion, &wsalib_d) != 0) {
		cerr << "WinSocket startup error!" << endl;
		return -1;
	}

	char name[MAX_NAME_SIZE];
	cout << "Input your nickname: ";
	cin.getline(name, MAX_NAME_SIZE);

	for (int i = 0; i < MAX_NAME_SIZE; i++) {
		if (name[i] == ' ') name[i] = '\0';
	}

	{
		std::string title = "title " + std::string(name);
		system(title.c_str());
	}

	std::string strAddress;
	cout << "Input server address (IP:PORT): ";
	cin >> strAddress;

	if (strAddress.find(':') == SIZE_MAX) {
		cerr << "Incorrect server address!" << endl;
		return -2;
	}

	Address address = makeAddress(strAddress);

	SOCKADDR_IN addr;
	addr.sin_addr.s_addr = inet_addr(address.ip.c_str());
	addr.sin_port = htons(address.port);
	addr.sin_family = AF_INET;
	size_t addrSize = sizeof(addr);

	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(serverSocket, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
		cerr << "Can't connect to the server!" << endl;
		return -2;
	}

	send(serverSocket, name, MAX_NAME_SIZE, NULL);
	cout << "Connection successful!" << endl;

	std::thread clientHandler(handleClient, &serverSocket);

	while (true) {
		Packet packet;
		cin.getline(packet.msg, MAX_MSG_SIZE);
		strcpy(packet.name, name);
		send(serverSocket, (char*)&packet, sizeof(packet), NULL);
	}

	system("pause");
	return 0;
}