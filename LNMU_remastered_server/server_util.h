struct Connection {
	std::string name;
	SOCKET socket = INVALID_SOCKET;
};

class ClientManager {
private:
	size_t maxClientCount;
	Connection* clients;
	std::queue<size_t> freeIndexes;
	std::mutex mtx;

public:
	ClientManager(size_t _maxClientCount);
	~ClientManager();

	size_t size() const;
	size_t clientCount() const;
	size_t addConnection(std::string name, SOCKET socket);

	void removeConnection(size_t _index);

	Connection getConnection(size_t index) const;

	void sendAll(char* msg, int len, int flags, size_t except) const;
	inline void sendTo(size_t index, char* msg, int len, int flags) const;

	size_t findByName(std::string name) const;
};

ClientManager::ClientManager(size_t _maxClientCount) : maxClientCount(_maxClientCount) {
	clients = new Connection[maxClientCount];
	
	for (size_t i = 0; i < maxClientCount; i++) {
		freeIndexes.push(i);
	}
}

ClientManager::~ClientManager() { delete[] clients; }

size_t ClientManager::size() const {
	return maxClientCount;
}

size_t ClientManager::clientCount() const {
	return maxClientCount - freeIndexes.size();
}

size_t ClientManager::addConnection(std::string name, SOCKET socket) {
	mtx.lock();

	if (freeIndexes.empty()) {
		mtx.unlock();
		return SIZE_MAX;
	}

	size_t index = freeIndexes.front();
	clients[index] = Connection{ name, socket };
	freeIndexes.pop();

	mtx.unlock();
	return index;
}

void ClientManager::removeConnection(size_t index) {
	mtx.lock();

	closesocket(clients[index].socket);
	clients[index].socket = INVALID_SOCKET;
	freeIndexes.push(index);

	mtx.unlock();
}

Connection ClientManager::getConnection(size_t index) const {
	return clients[index];
}

void ClientManager::sendAll(char* msg, int len, int flags, size_t except = SIZE_MAX) const {
	for (size_t i = 0; i < maxClientCount; i++) {
		if (i == except || clients[i].socket == INVALID_SOCKET) continue;
		send(clients[i].socket, msg, len, flags);
	}
}

inline void ClientManager::sendTo(size_t index, char* msg, int len, int flags) const {
	send(clients[index].socket, msg, len, flags);
}

size_t ClientManager::findByName(std::string name) const {
	for (size_t i = 0; i < maxClientCount; i++) {
		if (clients[i].socket != INVALID_SOCKET && clients[i].name == name) {
			return i;
		}
	}

	return SIZE_MAX;
}
