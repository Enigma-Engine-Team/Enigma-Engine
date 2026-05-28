#pragma once

#include "network/server.h"
#include "network/client.h"
#include "network/frame.h"
#include "utilities/macro.h"

class ENIGMA_API Network
{
public:
	static Network& GetInstance();

	void Create(std::string path);
	void Destroy();
	void Send();
	Frame Receive();
	void UpdateStatus(Client::EStatus status);
	void Update();
	Frame GetFrame();
	Client* GetClient();

private:
	Server* server = nullptr;
	Client* client = nullptr;
	int serverTask = 0;
	Frame currentFrame;

	void InitServer(int port);
	void InitClient(std::string ip, int port, std::string nickname);

	Network() = default;
	~Network() = default;
};