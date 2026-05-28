#pragma once

#include <enet/enet.h>
#include <vector>
#include <string>
#include "../utilities/macro.h"

class ENIGMA_API Server
{
public:
	Server() = default;
	~Server() = default;

	void Create(int port, int fps);
	void Destroy();

	void Send(std::string message);
	void Update();
	void Close();

private:
	ENetHost* server = nullptr;
	ENetAddress address;
	std::vector<ENetPeer*> peers;
	bool shouldClose = false;
	int timeUpdate = 0;
};